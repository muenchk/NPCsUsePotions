#pragma once

struct ID
{
	ID() = default;
	ID(RE::FormID id, RE::FormID originalID);
	ID(RE::FormID id);

	bool operator==(RE::FormID a_formID) const;
	bool operator==(ID& a_id) const;
	bool operator==(int a_formID) const;

	operator RE::FormID() const;

	void SetID(RE::FormID id);
	void SetOriginalID(RE::FormID originalID);
	void AddTemplateID(RE::FormID templateID);

	RE::FormID GetOriginalID();
	std::vector<RE::FormID> GetTemplateIDs();

private:
	RE::FormID _id;
	RE::FormID _originalID;
	std::vector<RE::FormID> _templateIDs;
};
