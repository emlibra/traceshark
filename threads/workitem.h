/*
 * Traceshark - a visualizer for visualizing ftrace and perf traces
 * Copyright (C) 2015, 2016, 2017  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 *
 *  a) This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of the
 *     License, or (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with this library; if not, write to the Free
 *     Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *     MA 02110-1301 USA
 *
 * Alternatively,
 *
 *  b) Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *     1. Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *     2. Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *     NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WORKITEM_H
#define WORKITEM_H

#include "misc/traceshark.h"

class WorkQueue;

/*
 * This class must be a virtual class, because it can't be a template, that 
 * would drag the WorkQueue class into the template business.
 */
class AbstractWorkItem {
	friend class WorkQueue;
public:
	virtual ~AbstractWorkItem() {}
protected:
	__always_inline bool __runWork();
	virtual bool run() = 0;
};

__always_inline bool AbstractWorkItem::__runWork() {
	return run();
}

/*
 * This class needs to be a template to be able to call functions in different
 * classes. The WorkQueue uses it through the AbstractWorkItem class interface
 * in order to avoid dealing with templates.
 */
template <class W>
class WorkItem : public AbstractWorkItem {
public:
	WorkItem(W *obj, DEFINE_MEMBER_FN(bool, W, fn));
	WorkItem() {}
	virtual ~WorkItem() {}
	void setObjFn(W *obj, DEFINE_MEMBER_FN(bool, W, fn));
protected:
	__always_inline bool run();
private:
	W *workObj;
	DEFINE_MEMBER_FN(bool, W,  workObjFn);
};

template <class W>
WorkItem<W>::WorkItem(W *obj, DEFINE_MEMBER_FN(bool, W, fn)):
workObj(obj), workObjFn(fn) {}

template <class W>
void WorkItem<W>::setObjFn(W *obj, DEFINE_MEMBER_FN(bool, W, fn)) {
	workObj = obj;
	workObjFn = fn;
}

template <class W>
__always_inline bool WorkItem<W>::run() {
	return CALL_MEMBER_FN(workObj, workObjFn)();
}

#endif /* WORKITEM_H */
