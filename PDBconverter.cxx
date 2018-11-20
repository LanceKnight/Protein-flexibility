#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkXMLPolyDataWriter.h>

#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <fstream>
#include <iostream>
#include <vtkGenericDataObjectReader.h>
#include <vtkPolyDataReader.h>
#include <vtkVRMLImporter.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkPointData.h>
#include <typeinfo>
int main(int argc, char *argv[])
{

  if ( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " firstProtein(.wrl) secondProtein(.wrl) outputImage" << std::endl; 
    return -1 ;
    }

	//read in the data from VRML file(.wrl)
	std::string input1 = argv[1];
	std::string input2 = argv[2];

                   	
/*
	//map the geometry to the graphics primitives
	vtkSmartPointer <vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	meshMapper->SetInputData(reader->GetOutput());
	

	//set up rendering properties
	vtkSmartPointer<vtkActor>meshActor=vtkSmartPointer<vtkActor>::New();
	meshActor->SetMapper(meshMapper);
*/

	//create scene
//	vtkSmartPointer<vtkRenderer>renderer=vtkSmartPointer<vtkRenderer>::New();
//	renderer->AddActor(meshActor);

	//create window
//	vtkSmartPointer <vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	vtkSmartPointer<vtkVRMLImporter> importer1 = vtkSmartPointer<vtkVRMLImporter>::New();
  importer1->SetFileName ( input1.c_str() );	
//  importer1->SetRenderWindow(renderWindow);
  importer1->Update();
 
	vtkSmartPointer < vtkRenderer > renderer1 = importer1->GetRenderer() ;
  vtkSmartPointer < vtkActorCollection > actors1 = renderer1->GetActors() ;
  actors1->InitTraversal () ;
  std::cout << "Number of protein 1 actors: " << actors1->GetNumberOfItems() << std::endl ; 

 	//Combining actors
 	vtkSmartPointer<vtkAppendPolyData> appendFilter1 = vtkSmartPointer<vtkAppendPolyData>::New(); 
 	for (int i = 0; i < (actors1->GetNumberOfItems()); i++){
			vtkSmartPointer < vtkActor > actor1 = actors1->GetNextActor() ;
			vtkSmartPointer < vtkMapper > mapper1 = actor1->GetMapper() ;
			vtkSmartPointer < vtkPolyData > dataset1 = dynamic_cast < vtkPolyData * > ( mapper1->GetInput() ) ;
			appendFilter1->AddInputData(dataset1);
	}
	appendFilter1->Update();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter1 =vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter1->SetInputConnection(appendFilter1->GetOutputPort());
  cleanFilter1->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputConnection(cleanFilter1->GetOutputPort());
 
  vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
  actor1->SetMapper(mapper1);
 
	vtkSmartPointer<vtkVRMLImporter> importer2 = vtkSmartPointer<vtkVRMLImporter>::New();
  importer2->SetFileName ( input2.c_str() );
  importer2->Update();

	//import protein 2
	vtkSmartPointer < vtkRenderer > renderer2 = importer2->GetRenderer() ;
  vtkSmartPointer < vtkActorCollection > actors2 = renderer2->GetActors() ;
  actors2->InitTraversal () ;
  std::cout << "Number of protein 2 actors: " << actors2->GetNumberOfItems() << std::endl ; 

 	//Combining actors for protein 2
 	vtkSmartPointer<vtkAppendPolyData> appendFilter2 = vtkSmartPointer<vtkAppendPolyData>::New(); 
 	for (int i = 0; i < (actors2->GetNumberOfItems()); i++){
			vtkSmartPointer < vtkActor > actor2 = actors2->GetNextActor() ;
			vtkSmartPointer < vtkMapper > mapper2 = actor2->GetMapper() ;
			vtkSmartPointer < vtkPolyData > dataset2 = dynamic_cast < vtkPolyData * > ( mapper2->GetInput() ) ;
			appendFilter2->AddInputData(dataset2);
	}
	appendFilter2->Update();

	//clean duplicate data for protein 2
	vtkSmartPointer<vtkCleanPolyData> cleanFilter2 =vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter2->SetInputConnection(appendFilter2->GetOutputPort());
  cleanFilter2->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper2->SetInputConnection(cleanFilter2->GetOutputPort());
 
  vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
  actor2->SetMapper(mapper2);


	//calcaulate the distance for between protein 1 and protein 2
  vtkSmartPointer<vtkDistancePolyDataFilter>distFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();
	vtkSmartPointer < vtkPolyData > dataset1 = dynamic_cast < vtkPolyData * > ( mapper1->GetInput() ) ;
	vtkSmartPointer < vtkPolyData > dataset2 = dynamic_cast < vtkPolyData * > ( mapper2->GetInput() ) ;
  distFilter->AddInputData(0,dataset1);
  distFilter->AddInputData(1,dataset2);
  distFilter->SignedDistanceOff();
  distFilter->Update();
	std::cout<<"finished calculating distance"<<std::endl;

	vtkSmartPointer<vtkPolyData> mesh = distFilter->GetOutput();
  double *range = mesh->GetPointData()->GetScalars()->GetRange();
  std::cout<<"Range:"<<range[0]<<" "<<range[1]<<std::endl;


	vtkSmartPointer <vtkPolyDataMapper> mapper_final = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper_final->SetInputData(distFilter->GetOutput());
  mapper_final->SetScalarVisibility(true);//same as scalarvisibilityon()
  mapper_final->SetScalarModeToUsePointData();
  mapper_final->SetColorModeToMapScalars();
  mapper_final->SetScalarRange(0.3, 4.5);

	//add legend
  vtkSmartPointer <vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
  scalarBar->SetLookupTable(mapper_final->GetLookupTable());
	scalarBar->SetTitle("Distance");
	
  vtkSmartPointer<vtkActor> actor_final = vtkSmartPointer<vtkActor>::New();
	actor_final ->SetMapper(mapper_final);

	vtkSmartPointer<vtkRenderer> renderer_final = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =vtkSmartPointer<vtkRenderWindow>::New();	
	renderWindow->SetSize(300,300);
  renderWindow->AddRenderer(renderer_final);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

	//add actor1 and actor2
	renderer_final->AddActor(actor_final);
	renderer_final->AddActor(scalarBar);
//	renderer_final->AddActor(actor2);
	std::cout << "Number of all actors after combination: " << renderer_final->GetActors()->GetNumberOfItems() << std::endl ; 
//	renderWindow->Render();
	renderWindowInteractor->Start();

  return 0;
}
