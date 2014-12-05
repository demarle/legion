/* Copyright 2014 Stanford University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <cstdio>

#include "legion.h"

// All of the important user-level objects live
// in the high-level runtime namespace.
using namespace LegionRuntime::HighLevel;
using namespace LegionRuntime::Accessor;

// We use an enum to declare the IDs for user-level tasks
enum TaskID {
  HELLO_WORLD_ID,
};

enum FieldIDs {
  FID_SCALAR,
  FID_R,
  FID_G,
  FID_B,
  FID_A,
  FID_Z
};

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);

#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRTAnalyticSource.h>
#include <vtkSmartPointer.h>

// All single-launch tasks in Legion must have this signature with
// the extension that they can have different return values.
void hello_world_task(const Task *task,
                      const std::vector<PhysicalRegion> &regions,
                      Context ctx, HighLevelRuntime *runtime)
{
  // A task runs just like a normal C++ function.
  /*
  std::cerr<< "Hello {" << std::endl;
  vtkSmartPointer<vtkImageData> id = vtkSmartPointer<vtkImageData>::New();
  id->PrintSelf(std::cerr, vtkIndent(7));
  std::cerr << "}" << std::endl;
  */

#define ISz 10
#define JSz 10
#define KSz 10

#if 0
  float *s = new float(ISz*JSz*Ksz);
#else
  Rect<1> rect(Point<1>(0),Point<1>(ISz*JSz*KSz-1));
  IndexSpace structured_is = runtime->create_index_space
    (ctx,
     Domain::from_rect<1>(rect));
  FieldSpace fs = runtime->create_field_space(ctx);
  FieldAllocator allocator = runtime->create_field_allocator(ctx, fs);
  FieldID fida = allocator.allocate_field(sizeof(float), FID_SCALAR);
  LogicalRegion structured_lr =
    runtime->create_logical_region(ctx, structured_is, fs);

  RegionRequirement req(structured_lr, READ_WRITE, EXCLUSIVE, structured_lr);
  req.add_field(FID_SCALAR);
  InlineLauncher input_launcher(req);
  PhysicalRegion input_region = runtime->map_region(ctx, input_launcher);
  input_region.wait_until_valid();

  RegionAccessor<AccessorType::Generic, float> acc_x =
    input_region.get_field_accessor(FID_SCALAR).typeify<float>();
  //write some test data

  int i = 0;
  int j = 0;
  int k = 0;
  for (GenericPointInRectIterator<1> pir(rect); pir; pir++) 
    {
    k = k + 1;
    if (k == KSz)
      {
      k = 0;
      j = j + 1;
      if (j == JSz)
        {
        j = 0;
        i = i + 1;
        }
      }
    acc_x.write(DomainPoint::from_point<1>(pir.p), i+j+k);
  }
  /*
  Rect<1> subrect;
  ByteOffset off(0); 
  void *data = acc_x.raw_rect_ptr<1>(rect, subrect, &off);
  cerr << ((float*)data)[1000] << endl;
  */

  //get raw pointer, is there a safer way? VTK will want raw pointers
  Rect<1> subrect;
  ByteOffset off(0); 
  void *data = acc_x.raw_rect_ptr<1>(rect, subrect, &off);

#endif


  vtkSmartPointer<vtkRTAnalyticSource> idp = vtkSmartPointer<vtkRTAnalyticSource>::New();
  vtkSmartPointer<vtkContourFilter> cf = vtkSmartPointer<vtkContourFilter>::New();
  cf->SetInputConnection(idp->GetOutputPort());
  cf->SetNumberOfContours(1);
  cf->SetValue(0,125);
  //cf->Update();
  //cf->GetOutput()->PrintSelf(std::cerr, vtkIndent(0));
  vtkSmartPointer<vtkDataSetMapper> m = vtkSmartPointer<vtkDataSetMapper>::New();
  m->SetInputConnection(cf->GetOutputPort());
  vtkSmartPointer<vtkActor> a = vtkSmartPointer<vtkActor>::New();
  a->SetMapper(m);
  vtkSmartPointer<vtkRenderer> r = vtkSmartPointer<vtkRenderer>::New();
  r->AddActor(a);
  vtkSmartPointer<vtkRenderWindow> rw = vtkSmartPointer<vtkRenderWindow>::New();
  rw->AddRenderer(r);
  rw->Render();
  r->ResetCamera();
  rw->Render();
  //std::string s;
  //cerr << "ANY KEY TO CONTINUE" << endl;
  //cin >> s;
  int *wh = rw->GetSize();
  int w = wh[0];
  int h = wh[1];
  float *rgb = new float[w*h*4];
  int slop = 150; //wtf?
  float *z = new float[w*h+slop];

  vtkSmartPointer<vtkFloatArray> fa = vtkSmartPointer<vtkFloatArray>::New();
  fa->SetNumberOfComponents(4);
  fa->SetNumberOfTuples(w*h);

  bool legion_owns = true; //then vtk won't free when it goes away
  fa->SetVoidArray(rgb, w*h*4*sizeof(float), legion_owns);

  rw->GetRGBAPixelData(0,0,w,h, 1, fa);
  rw->GetZbufferData(0,0,w,h, z);

  for (int hi = 0; hi < h; hi+=50)
    {
    for (int wi = 0; wi < w; wi+=50)
      {
      int pxoffset = (hi*w + wi);
      cerr << rgb[pxoffset*4+0] << " "
           << rgb[pxoffset*4+1] << " "
           << rgb[pxoffset*4+2] << " "
           << rgb[pxoffset*4+3] << " "
           << z[pxoffset]
           << " ";
      }
    cerr << endl;
    }

  runtime->destroy_logical_region(ctx, structured_lr);
  runtime->destroy_field_space(ctx, fs);
  runtime->destroy_index_space(ctx, structured_is);

}

// We have a main function just like a standard C++ program.
// Once we start the runtime, it will begin running the top-level task.
int main(int argc, char **argv)
{
  // Before starting the Legion runtime, you first have to tell it
  // what the ID is for the top-level task.
  HighLevelRuntime::set_top_level_task_id(HELLO_WORLD_ID);
  // Before starting the Legion runtime, all possible tasks that the
  // runtime can potentially run must be registered with the runtime.
  // The function pointer is passed as a template argument.  The second
  // argument specifies the kind of processor on which the task can be
  // run: latency optimized cores (LOC) aka CPUs or throughput optimized
  // cores (TOC) aka GPUs.  The last two arguments specify whether the
  // task can be run as a single task or an index space task (covered
  // in more detail in later examples).  The top-level task must always
  // be able to be run as a single task.
  HighLevelRuntime::register_legion_task<hello_world_task>(HELLO_WORLD_ID,
      Processor::LOC_PROC, true/*single*/, false/*index*/);

  // Now we're ready to start the runtime, so tell it to begin the
  // execution.  We'll never return from this call, but its return
  // signature will return an int to satisfy the type checker.
  return HighLevelRuntime::start(argc, argv);
}
