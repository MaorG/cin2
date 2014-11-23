#include "Classifier.h"
//#include "PolyLineEntity.h"
//#include "PolyLineProcessor.h"
//
//Model* Classifier::getPreprocessedModel(Model *model) {
//	Model * processedModel = new Model();
//
//	std::vector<Entity*> * entities = model->getEntities();
//
//	//isPolyLineEntity
//	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();
//
//
//	for (std::vector<Entity*>::iterator it = entities->begin() + 1; it != entities->end(); ++it) {
//
//		if ((*it)->isPolyLineEntity()) {
//			PolyLineEntity* polyLineEntity = (PolyLineEntity*)(*it);
//			PolyLineProcessor::chainPolyLines(first, polyLineEntity);
//		}
//	}
//
//	processedModel->addEntity(first);
//	processedModel->normalizeBoundingBox();
//	first = (PolyLineEntity*)processedModel->getEntityByIndex(0);
//	processedModel->popEntity();
//
//	PolyLineEntity* result = PolyLineProcessor::prepareForNN(first, true, m_sampleSize);
//	delete first;
//	processedModel->addEntity(result);
//	return processedModel;
//}