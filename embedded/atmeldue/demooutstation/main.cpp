/*
 * demooutstation.cpp
 *
 * Created: 8/27/2014 7:55:11 AM
 *  Author: jadamcrain
 */ 

#include "sam.h"

#include <opendnp3/transport/TransportStack.h>
#include <opendnp3/outstation/Outstation.h>
#include <opendnp3/outstation/DynamicallyAllocatedDatabase.h>
#include <opendnp3/outstation/DynamicallyAllocatedEventBuffer.h>
#include <opendnp3/outstation/IOutstationApplication.h>

#include <openpal/logging/LogRoot.h>

#include "ExecutorImpl.h"
#include "LinkParserImpl.h"
#include "CommandHandlerImpl.h"
#include "Macros.h"

using namespace opendnp3;
using namespace openpal;

void ToggleBinaryEvery3Seconds(IExecutor* pExecutor, Database* pDatabase, uint8_t index = 0, bool value = true);

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
    // Initialize the SAM system
    SystemInit();
	
	ExecutorImpl exe(5,5);
	
	LogRoot root(nullptr, "root", 0);
	
	TransportStack stack(root, &exe, 2048, nullptr, LinkConfig(false, false));
	
	// 5 static binaries, 0 others
	DynamicallyAllocatedDatabase buffers(5);
	// allow a max of 5 events
	DynamicallyAllocatedEventBuffer eventBuffers(5);
	
	
	Database database(buffers.GetFacade());
	
	OutstationConfig config;
	config.eventBufferConfig = EventBufferConfig(5);
	config.params.allowUnsolicited = true;
	config.defaultEventResponses.binary = EventBinaryResponse::Group2Var2;
	
	// Object that handles command (CROB / analog output) requests
	// This example can toggle an LED
	CommandHandlerImpl commandHandler;
	
	Outstation outstation(config, exe, root, stack.transport, commandHandler, DefaultOutstationApplication::Instance(), database, eventBuffers.GetFacade());
	
	stack.transport.SetAppLayer(&outstation);
	
	LinkParserImpl parser(root, exe, stack.link);
	stack.link.SetRouter(&parser);
	stack.link.OnLowerLayerUp();
	
	// enable timer interrupts at 100Hz
	exe.Init();
	
	// enable USART RX/TX interrupts
	parser.Init();	
	
	ToggleBinaryEvery3Seconds(&exe, &database);
	
	for (;;)
	{
		// process any bytes that were received on the interrupt
		parser.ProcessRx();

		// run all pending events or expired timers
		exe.Run();
		
		// sleep until an interrupt occurs
		exe.Sleep();
	}

	return 0;
}

void ToggleBinaryEvery3Seconds(IExecutor* pExecutor, Database* pDatabase, uint8_t index, bool value)
{
	uint16_t next = ((index + 1) % 5);
	
	{
		Transaction tx(pDatabase);
		pDatabase->Update(Binary(value, 0x01, pExecutor->GetTime().milliseconds), index);
	}
	
	auto lambda = [pExecutor, pDatabase, value, next]() { ToggleBinaryEvery3Seconds(pExecutor, pDatabase, next, !value); };
	pExecutor->Start(TimeDuration::Seconds(3), Action0::Bind(lambda));
}

