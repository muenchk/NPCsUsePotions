#pragma once

struct ID
{
	ID() = default;
	/// <summary>
	/// Constructs an ID from the active FormID and the FormID of the original ActorBase
	/// </summary>
	/// <param name="id">IS of the RE::Actor*</param>
	/// <param name="originalID">ID of the original template</param>
	ID(RE::FormID id, RE::FormID originalID);
	/// <summary>
	/// Constructs an ID from an actor
	/// </summary>
	/// <param name="actor">The actor</param>
	ID(RE::Actor* actor);
	/// <summary>
	/// Constructs an ID from the actors FormID
	/// </summary>
	/// <param name="id"></param>
	ID(RE::FormID id);

	bool operator==(RE::FormID a_formID) const;
	bool operator==(ID& a_id) const;
	bool operator==(int a_formID) const;

	operator RE::FormID() const;

	/// <summary>
	/// Sets the FormID of the actor
	/// </summary>
	/// <param name="id"></param>
	void SetID(RE::FormID id);
	/// <summary>
	/// Sets the FormID of the actors original template
	/// </summary>
	/// <param name="originalID"></param>
	void SetOriginalID(RE::FormID originalID);
	/// <summary>
	/// Adds the ID of a template to the list of template IDs
	/// </summary>
	/// <param name="templateID"></param>
	void AddTemplateID(RE::FormID templateID);

	/// <summary>
	/// Returns the ID of the actors original template
	/// </summary>
	/// <returns></returns>
	RE::FormID GetOriginalID();
	/// <summary>
	/// Returns the list of IDs of template of the actor
	/// </summary>
	/// <returns></returns>
	std::vector<RE::FormID> GetTemplateIDs();

private:
	/// <summary>
	/// FormID of the actor
	/// </summary>
	RE::FormID _id;
	/// <summary>
	/// FormID of the actors original template
	/// </summary>
	RE::FormID _originalID;
	/// <summary>
	/// FormIDs of the actors templates
	/// </summary>
	std::vector<RE::FormID> _templateIDs;
};
