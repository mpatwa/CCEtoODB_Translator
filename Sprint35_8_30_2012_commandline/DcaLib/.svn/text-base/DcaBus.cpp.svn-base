
#include "StdAfx.h"
#include "DcaBus.h"
#include "DcaTMatrix.h"
#include "DcaAttributes.h"

//_____________________________________________________________________________
CBusStruct::CBusStruct(CCamCadData& camCadData,CString name, long entityNumber)
: CEntityWithAttributes(camCadData,true, entityNumber)
{
	this->m_className = "CBusStruct";
	this->SetName(name);
}

CBusStruct::~CBusStruct()
{
}

void CBusStruct::AddNetName(CString netName)
{
	if (netName.IsEmpty() == true)
		return;

	if (m_netNameList.Find(netName) != NULL)
		return;

	m_netNameList.AddTail(netName);
}

void CBusStruct::ReplaceNetName(CString oldName, CString newName)
{
	if (newName.IsEmpty())
		return;

	POSITION pos = m_netNameList.Find(oldName);
	if (pos != NULL)
	{
		if (m_netNameList.Find(newName) == NULL)
		{
			// If the new netname is not in the list then replace the old netname with the new netname
			m_netNameList.SetAt(pos, newName);
		}
		else
		{
			// If the new netname is in the list, then delete the old netname
			m_netNameList.RemoveAt(pos);
		}
	}
}

void CBusStruct::AddBusName(CString busName)
{
	if (busName.IsEmpty() == true)
		return;

	if (m_busNameList.Find(busName) != NULL)
		return;

	m_busNameList.AddTail(busName);
}

void CBusStruct::ReplaceBusName(CString oldName, CString newName)
{
	if (newName.IsEmpty())
		return;

	POSITION pos = m_busNameList.Find(oldName);
	if (pos != NULL)
	{
		if (m_busNameList.Find(newName) == NULL)
		{
			// If the new busname is not in the list then replace the old busname with the new busname
			m_busNameList.SetAt(pos, newName);
		}
		else
		{
			// If the new busname is in the list, then delete the old busname
			m_busNameList.RemoveAt(pos);
		}
	}
}

void CBusStruct::CopyBusAndNetNameList(CBusStruct* fromBus)
{
	POSITION pos = fromBus->GetHeadPositionBusName();
	while (pos)
		this->AddBusName(fromBus->GetNextBusName(pos));

	pos = fromBus->GetHeadPositionNetName();
	while (pos)
		this->AddNetName(fromBus->GetNextNetName(pos));
}

//_____________________________________________________________________________
CBusList::CBusList(CCamCadData& camCadData)
: m_camCadData(camCadData)
{
};

CBusList::~CBusList()
{
}

CCamCadData& CBusList::getCamCadData() const
{
   return m_camCadData;
}

POSITION CBusList::GetHeadPosition() const
{
   return m_busList.GetHeadPosition();
}

CBusStruct* CBusList::GetNext(POSITION &pos) const
{
   return m_busList.GetNext(pos);
}

int CBusList::GetCount() const
{
   return m_busList.GetCount();
}

CBusStruct* CBusList::Find(CString busName)
{
	POSITION pos = m_busList.GetHeadPosition();
	while (pos != NULL)
	{
		CBusStruct *bus = m_busList.GetNext(pos);
		if (bus != NULL && bus->GetName().CompareNoCase(busName) == 0)
			return bus;
	}

	return NULL;
}

bool CBusList::RemoveBus(CBusStruct* bus)
{
	m_busList.RemoveAt(m_busList.Find(bus));
	delete bus;
	bus = NULL;
	return true;
}

CBusStruct* CBusList::AddBus(CString busName, long entityNumber)
{
	if (busName.IsEmpty())
		return NULL;

	CBusStruct *bus = Find(busName);

	if (bus == NULL)
	{
		// The bus doesn't exist so create a new one
		bus = new CBusStruct(getCamCadData(),busName, entityNumber);
		m_busList.AddTail(bus);
	}

	return bus;
}

void CBusList::AddBus(CBusStruct *bus)
{
	if (m_busList.Find(bus))
		return;

	m_busList.AddTail(bus);
}

void CBusList::FreeBusList()
{
   m_busList.empty();
}

void CBusList::Scale(double factor)
{
	CTMatrix mat;
	mat.scale(factor, factor);

	POSITION busPos = m_busList.GetHeadPosition();
	while (busPos)
	{
		CBusStruct* bus = m_busList.GetNext(busPos);
		if (bus == NULL)
			continue;

		if (bus->getAttributes() != NULL)
			bus->getAttributes()->transform(mat);
	}
}
