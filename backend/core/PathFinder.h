/*
 Copyright (c) 2012 Petr Koupy

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <vector>
#include <ctime>

#include <tbb/task.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/atomic.h>
#include <tbb/parallel_scan.h>
#include <tbb/parallel_do.h>

#include "MolpherParam.h"
#include "MolpherMolecule.h"
#include "IterationSnapshot.h"

class PathFinder
{
public:
    PathFinder(tbb::task_group_context *tbbCtx,
            IterationSnapshot &iterationSnapshot, int threadCnt);
    ~PathFinder();
    void operator()();
private:
    tbb::task_group_context *mTbbCtx;
    IterationSnapshot &mSnp;
    int mThreadCnt;
};
