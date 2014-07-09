#include "BehaviorComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BlackBoard.h"

const char *BehaviorComponent::s_name = "BehaviorComponent";

BehaviorComponent::BehaviorComponent()
	: m_root(nullptr)
{
	this->setName(BehaviorComponent::s_name);
}

BehaviorComponent::~BehaviorComponent()
{

}

void BehaviorComponent::loadBehavior( const char *filename )
{
	m_root = BTReader::getInstance()->readFromFile(filename);
}

bool BehaviorComponent::evaluateBehavior( const BlackBoard &inputData )
{
	const BTInputParam &input = BTInputParam((void *)&inputData);
	return m_root && m_root->evaluate(input);
}

void BehaviorComponent::updateBehavior( const BlackBoard &inputData, BlackBoard &outputData )
{
	const BTInputParam &input = BTInputParam((void *)&inputData);
	BTInputParam &output = BTOutputParam((void *)&outputData);
	m_root->update(input, output);
}

