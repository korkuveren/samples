/*
	This template function exists to traverse a node graph to find some node type we've specified. 
	This is particularly useful to "reach through" nodes which act as a proxy to some other node, functionally. 
	One such example would be a ternary node.
	
	Certain types have been changed/redefined, and some internals have been removed. 	
 */
 
/**
 * Track down any node that is connected in the direction of the provided pin, recursively. 
 * 
 * @param SelfPin The pin to start searching from.
 * @param bByPinType Whether to only search pins which match the same type. Its recommended to pass this as true, or quite a few nodes will be processed.
 * @param bRequireUnique Whether the results should be unique - increases complexity, but if you're going to filter anyway may as well handle this now.
 * @return The found nodes.
 */
template<typename OtherNodeType>
std::vector<OtherNodeType*> FindConnectedNodesOfTypeFromPin(PinType* SelfPin, bool bByPinType = true, bool bRequireUnique = true)
{
	std::vector<OtherNodeType*> Return = {};

	if (SelfPin)
	{
		EDirectionEnum SelfDirection = SelfPin->Direction;

		for (int32 i = 0; i < SelfPin->LinkedTo.Num(); ++i)
		{
			PinType* Other = SelfPin->LinkedTo[i];
			if (Other)
			{
				ActualNodeType* OtherNode = Other->GetOwnerNode();
				if (!OtherNode) continue;
				if (OtherNodeType* FoundNodeType = Cast<OtherNodeType>(OtherNode))
				{
					// @note this also checked for a duplicate in the original code
					Return.push_back(FoundNodeType);
				}

				for (int32 j = 0; j < OtherNode->Pins.Num(); ++j)
				{
					PinType* OtherNodePin = OtherNode->Pins[j];
					if (bByPinType)
					{
#define ComparePinTypeMacro(Thing) OtherNodePin->Thing != SelfPin->Thing
						// omitted
#undef ComparePinTypeMacro
					}
					if (OtherNodePin->Direction != SelfDirection) continue;

					/**	Go find the connected nodes to the node we're connected to. */
					std::vector<OtherNodeType*> RecursivelyFound = FindConnectedNodesOfTypeFromPin<OtherNodeType>(OtherNodePin, bByPinType, bRequireUnique);
					
					if (bRequireUnique)
					{
						for (auto& Found : RecursivelyFound)
						{
							// @note this also checked for a duplicate in the original code
							Return.push_back(Found);
						}
					}
					else
					{
						Return.insert(Return.end(), RecursivelyFound.begin(), RecursivelyFound.end());
					}
				}
			}
		}
	}

	return Return;
}