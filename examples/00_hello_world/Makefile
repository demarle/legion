# Copyright 2014 Stanford University
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


ifndef LG_RT_DIR
$(error LG_RT_DIR variable is not defined, aborting build)
endif

#Flags for directing the runtime makefile what to include
DEBUG=1                   # Include debugging symbols
OUTPUT_LEVEL=LEVEL_DEBUG  # Compile time print level
SHARED_LOWLEVEL=1	  # Use the shared low level
#ALT_MAPPERS=1		  # Compile the alternative mappers

# Put the binary file name here
OUTFILE		:= hello_world
# List all the application source files here
GEN_SRC		:= hello_world.cc		# .cc files
GEN_GPU_SRC	:=				# .cu files

# You can modify these variables, some will be appended to by the runtime makefile

#from: /Source/VTK/devel/VTK/Utilities/Maintenance/WhatModulesVTK.py /Source/VTK/devel/VTK /Source/LEGION/src/legion/examples/00_hello_world/hello_world.cc, add the src&bld to -I and libs to -L
INC_FLAGS	:=\
-I /Source/VTK/devel/VTK/Common/Core \
-I /Source/VTK/devel/VTK/Common/DataModel \
-I /Source/VTK/devel/VTK/Common/ExecutionModel \
-I /Source/VTK/devel/VTK/Common/Math \
-I /Source/VTK/devel/VTK/Common/Misc \
-I /Source/VTK/devel/VTK/Common/System \
-I /Source/VTK/devel/VTK/Common/Transforms \
-I /Source/VTK/devel/VTK/Filters/Core \
-I /Source/VTK/devel/VTK/Imaging/Core \
-I /Source/VTK/devel/VTK/Rendering/Core \
-I /Source/VTK/devel/VTK/Rendering/OpenGL \
-I /Builds/VTK/devel/master/Common/Core \
-I /Builds/VTK/devel/master/Common/DataModel \
-I /Builds/VTK/devel/master/Common/ExecutionModel \
-I /Builds/VTK/devel/master/Common/Math \
-I /Builds/VTK/devel/master/Common/Misc \
-I /Builds/VTK/devel/master/Common/System \
-I /Builds/VTK/devel/master/Common/Transforms \
-I /Builds/VTK/devel/master/Filters/Core \
-I /Builds/VTK/devel/master/Imaging/Core \
-I /Builds/VTK/devel/master/Rendering/Core \
-I /Builds/VTK/devel/master/Rendering/OpenGL


CC_FLAGS	:=
NVCC_FLAGS	:=
GASNET_FLAGS	:=
LD_FLAGS	:=

###########################################################################
#
#   Don't change anything below here
#   
###########################################################################

# All these variables will be filled in by the runtime makefile
LOW_RUNTIME_SRC	:=
HIGH_RUNTIME_SRC:=
GPU_RUNTIME_SRC	:=
MAPPER_SRC	:=

include $(LG_RT_DIR)/runtime.mk

# General shell commands
SHELL	:= /bin/sh
SH	:= sh
RM	:= rm -f
LS	:= ls
MKDIR	:= mkdir
MV	:= mv
CP	:= cp
SED	:= sed
ECHO	:= echo
TOUCH	:= touch
MAKE	:= make
ifndef GCC
GCC	:= g++
endif
ifndef NVCC
NVCC	:= $(CUDA)/bin/nvcc
endif
SSH	:= ssh
SCP	:= scp

common_all : all

.PHONY	: common_all

GEN_OBJS	:= $(GEN_SRC:.cc=.o)
LOW_RUNTIME_OBJS:= $(LOW_RUNTIME_SRC:.cc=.o)
HIGH_RUNTIME_OBJS:=$(HIGH_RUNTIME_SRC:.cc=.o)
MAPPER_OBJS	:= $(MAPPER_SRC:.cc=.o)
# Only compile the gpu objects if we need to 
ifndef SHARED_LOWLEVEL
GEN_GPU_OBJS	:= $(GEN_GPU_SRC:.cu=.o)
GPU_RUNTIME_OBJS:= $(GPU_RUNTIME_SRC:.cu=.o)
else
GEN_GPU_OBJS	:=
GPU_RUNTIME_OBJS:=
endif

VTKLIBS := \
/Builds/VTK/devel/master/lib/libvtkCommonCore.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonDataModel.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonExecutionModel.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonMath.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonMisc.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonSystem.dylib \
/Builds/VTK/devel/master/lib/libvtkCommonTransforms.dylib \
/Builds/VTK/devel/master/lib/libvtkFiltersCore.dylib \
/Builds/VTK/devel/master/lib/libvtkImagingCore.dylib \
/Builds/VTK/devel/master/lib/libvtkRenderingCore.dylib \
/Builds/VTK/devel/master/lib/libvtkRenderingOpenGL.dylib

ALL_OBJS	:= $(GEN_OBJS) $(GEN_GPU_OBJS) $(LOW_RUNTIME_OBJS) $(HIGH_RUNTIME_OBJS) $(GPU_RUNTIME_OBJS) $(MAPPER_OBJS)

all:
	$(MAKE) $(OUTFILE)

# If we're using the general low-level runtime we have to link with nvcc
$(OUTFILE) : $(ALL_OBJS)
	@echo "---> Linking objects into one binary: $(OUTFILE)"
ifdef SHARED_LOWLEVEL
	$(GCC) -o $(OUTFILE) $(ALL_OBJS) $(LD_FLAGS) $(GASNET_FLAGS) $(VTKLIBS)
else
	$(NVCC) -o $(OUTFILE) $(ALL_OBJS) $(LD_FLAGS) $(GASNET_FLAGS)
endif

$(GEN_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(LOW_RUNTIME_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(HIGH_RUNTIME_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(MAPPER_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(GEN_GPU_OBJS) : %.o : %.cu
	$(NVCC) -o $@ -c $< $(INC_FLAGS) $(NVCC_FLAGS)

$(GPU_RUNTIME_OBJS): %.o : %.cu
	$(NVCC) -o $@ -c $< $(INC_FLAGS) $(NVCC_FLAGS)

clean:
	@$(RM) -rf $(ALL_OBJS) $(OUTFILE)
