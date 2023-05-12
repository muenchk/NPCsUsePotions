#include "ID.h"

ID::ID(RE::FormID id, RE::FormID originalID) :
	_id(id),
	_originalID(originalID)
{

}

ID::ID(RE::FormID id) :
	_id(id)
{
	_originalID = 0;
}

ID::ID(RE::Actor* actor)
{
	_id = actor->GetFormID();
	_originalID = 0;
	// get original id
	if (const auto extraLvlCreature = actor->extraList.GetByType<RE::ExtraLeveledCreature>()) {
		if (const auto originalBase = extraLvlCreature->originalBase) {
			_originalID = (originalBase->GetFormID());
		}
		if (const auto templateBase = extraLvlCreature->templateBase) {
			_templateIDs.push_back(templateBase->GetFormID());
		}
	} else {
		_originalID = (actor->GetActorBase()->GetFormID());
	}
}

bool ID::operator==(RE::FormID a_formID) const
{
	if (_id == a_formID || _originalID == a_formID)
		return true;
	for (auto& id : _templateIDs)
		if (id == a_formID)
			return true;
	return false;
}

bool ID::operator==(int a_formID) const
{
	if (_id == (unsigned int)a_formID || _originalID == (unsigned int)a_formID)
		return true;
	for (auto& id : _templateIDs)
		if (id == (unsigned int)a_formID)
			return true;
	return false;
}

bool ID::operator==(ID& a_id) const
{
	if (a_id._id == _id && a_id._originalID == _originalID)
		return true;
	return false;
}

ID::operator RE::FormID() const
{
	return _id;
}

void ID::SetID(RE::FormID id)
{
	_id = id;
}

void ID::SetOriginalID(RE::FormID originalID)
{
	_originalID = originalID;
}

void ID::AddTemplateID(RE::FormID templateID)
{
	_templateIDs.push_back(templateID);
}

RE::FormID ID::GetOriginalID()
{
	return _originalID;
}

std::vector<RE::FormID> ID::GetTemplateIDs()
{
	return _templateIDs;
}
