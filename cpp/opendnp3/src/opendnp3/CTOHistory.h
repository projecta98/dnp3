
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

#ifndef __CTO_HISTORY_H_
#define __CTO_HISTORY_H_

#include <openpal/Visibility.h>

namespace opendnp3
{

class DLL_LOCAL CTOHistory
{
public:
	CTOHistory() :
		mCTO(-1),
		mHeaderNum(0),
		mTimeHeaderNum(0) {

	}

	void Reset() {
		mCTO = -1;
		mHeaderNum = 0;
		mTimeHeaderNum = 0;
	}

	void NextHeader() {
		++mHeaderNum;
	}

	void SetCTO(int64_t aTime) {
		mCTO = aTime;
		mTimeHeaderNum = mHeaderNum;
	}

	bool GetCTO(int64_t& arTime) {
		if(mCTO >= 0 && mTimeHeaderNum == (mHeaderNum - 1)) {
			arTime = mCTO;
			return true;
		}
		else return false;
	}

private:

	int64_t mCTO;
	size_t mHeaderNum;
	size_t mTimeHeaderNum;

};

}

#endif