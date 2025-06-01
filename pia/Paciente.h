#pragma once
#include "Medico.h"

enum SEXO {
	FEMENINO = 0,
	MASCULINO = 1
};

struct Paciente {
	int idPaciente; // PK
	char nombre[50];
	char apellidoPaterno[50];
	char apellidoMaterno[50];
	char correo[50];
	long telefono;
	SYSTEMTIME fechaNacimiento;
	SEXO sexo;

	bool estatus;

	Paciente* siguiente = NULL;
	Paciente* anterior = NULL;
};

Paciente* primeroPaciente = NULL, * ultimoPaciente = NULL;
char archivoPacientes[MAX_PATH];

Paciente* crearPaciente(int idPaciente, char nombre[50], char apellidoPaterno[50], char apellidoMaterno[50], char correo[50], long telefono, SYSTEMTIME fechaNacimiento, SEXO sexo, bool estatus) {
	Paciente* nuevoPaciente = new Paciente;
	nuevoPaciente->idPaciente = idPaciente;
	strcpy(nuevoPaciente->nombre, nombre);
	strcpy(nuevoPaciente->apellidoPaterno, apellidoPaterno);
	strcpy(nuevoPaciente->apellidoMaterno, apellidoMaterno);
	strcpy(nuevoPaciente->correo, correo);
	nuevoPaciente->telefono = telefono;
	nuevoPaciente->fechaNacimiento = fechaNacimiento;
	nuevoPaciente->sexo = sexo;
	nuevoPaciente->estatus = estatus;
	nuevoPaciente->siguiente = NULL;
	nuevoPaciente->anterior = NULL;
	return nuevoPaciente;
}

void agregarPaciente(Paciente* nuevo) {
	if (primeroPaciente == NULL) {
		primeroPaciente = nuevo;
		ultimoPaciente = nuevo;
	}
	else {
		ultimoPaciente->siguiente = nuevo;
		nuevo->anterior = ultimoPaciente;
		ultimoPaciente = nuevo;
	}
}

Paciente* buscarPaciente(int idPaciente) {
	Paciente* actual = primeroPaciente;
	while (actual != NULL) {
		if (actual->idPaciente == idPaciente) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Paciente* obtenerUltimoPaciente()
{
	Paciente* temporal = primeroPaciente;
	while (temporal->siguiente)
		temporal = temporal->siguiente;
	return temporal;
}


// Método de ordenamiento heapsort para la lista de pacientes
Paciente* HeapInsert(Paciente* pHead, Paciente* pNew)
{
	if (pHead == NULL)
	{
		pNew->anterior = pNew->siguiente = NULL;
		return pNew;
	}
	
	if (strcmp(pNew->apellidoPaterno, pHead->apellidoPaterno) < 0) // era '>'
	{
		/* switch A/B to try to maintain balance */
		pNew->siguiente = pHead->anterior;
		pNew->anterior = HeapInsert(pHead->siguiente, pHead);
		return pNew;
	}
	else
	{
		pHead->anterior = HeapInsert(pHead->anterior, pNew);
		return pHead;
	}
}

Paciente* HeapRemove(Paciente* pHead)
{
	if (pHead == NULL)
		return NULL;

	if (pHead->anterior == NULL)
		return pHead->siguiente;
	if (pHead->siguiente == NULL)
		return pHead->anterior;
	
	if (strcmp(pHead->anterior->apellidoPaterno, pHead->siguiente->apellidoPaterno) < 0) // era '>'
	{
		pHead->anterior->anterior = HeapRemove(pHead->anterior);
		pHead->anterior->siguiente = pHead->siguiente;
		return pHead->anterior;
	}
	else
	{
		pHead->siguiente->siguiente = HeapRemove(pHead->siguiente);
		pHead->siguiente->anterior = pHead->anterior;
		return pHead->siguiente;
	}
}

Paciente* HeapSort(Paciente* pList)
{
	Paciente* pIter = pList, * pNext, * pPrev;
	Paciente* pHead = NULL;
	/* build heap */
	while (pIter)
	{
		/* take one out of the list */
		pNext = pIter->siguiente;
		/* put it into the heap */
		pHead = HeapInsert(pHead, pIter);
		pIter = pNext;
	}

	/* tear down heap */
	pPrev = NULL;
	while (pHead)
	{
		/* take one out of the heap */
		pIter = pHead;
		pHead = HeapRemove(pHead);
		/* put it into the list */
		pIter->anterior = pPrev;
		if (pPrev == NULL)
			pList = pIter;
		else
			pPrev->siguiente = pIter;
		pPrev = pIter;
	}
	if (pIter)
		pIter->siguiente = NULL;
	return pList;
}

void escribirPacientes(std::ofstream& archivo) {
	Paciente* actual = primeroPaciente;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Paciente));
		actual = actual->siguiente;
	}
}

void llenarListaPacientes(HWND hListBox, Paciente* lista) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	while (lista != NULL) {
		char buffer[150];
		sprintf_s(buffer, "%d - %s %s %s", lista->idPaciente, lista->nombre, lista->apellidoPaterno, lista->apellidoMaterno);
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
		lista = lista->siguiente;
	}
}


void leerPacientes(std::ifstream& archivo) {
	while (true)
	{
		Paciente* nuevo = new Paciente;
		archivo.read(reinterpret_cast<char*>(nuevo), sizeof(Paciente));

		if (archivo.eof())
		{
			delete nuevo; // libreando la memoria que creamos y no utilizamos del puntero 'nuevo'
			break;
		}

		agregarPaciente(nuevo);
	}
}