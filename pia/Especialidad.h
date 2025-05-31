#pragma once
#include "Medico.h"

struct Especialidad {
	int idEspecialidad; // PK
	char nombre[50];

	Especialidad* izquierdo = NULL;
	Especialidad* derecho = NULL;
};

Especialidad* raiz = NULL;
char archivoEspecialidades[MAX_PATH];

Especialidad* crearEspecialidad(int idEspecialidad, char nombre[50]) {
	Especialidad* nuevaEspecialidad = new Especialidad;
	nuevaEspecialidad->idEspecialidad = idEspecialidad;
	strcpy(nuevaEspecialidad->nombre, nombre);
	nuevaEspecialidad->izquierdo = NULL;
	nuevaEspecialidad->derecho = NULL;
	return nuevaEspecialidad;
}

void agregarEspecialidad(Especialidad*& actual, Especialidad*& nueva) {
	if (raiz == NULL) {
		raiz = nueva;
	}
	else {

		if (nueva->idEspecialidad < actual->idEspecialidad) {
			if (actual->izquierdo == NULL) {
				actual->izquierdo = nueva;
			}
			else {
				agregarEspecialidad(actual->izquierdo, nueva);
			}
		}
		else {
			if (actual->derecho == NULL) {
				actual->derecho = nueva;
			}
			else {
				agregarEspecialidad(actual->derecho, nueva);
			}
		}
	}
}

Especialidad* buscarEspecialidad(Especialidad*& actual, int idEspecialidad) {

	if (actual == NULL)
		return NULL;

	if (actual->idEspecialidad == idEspecialidad)
		return actual;
	
	if (actual->idEspecialidad > idEspecialidad)
		return buscarEspecialidad(actual->izquierdo, idEspecialidad);
	else
		return buscarEspecialidad(actual->derecho, idEspecialidad);
}

// Llenar lista con especialidades actuales
void recorrerInorden(Especialidad* nodo, HWND hLista) {
	if (!nodo) return;
	recorrerInorden(nodo->izquierdo, hLista);

	char buffer[100];
	//sprintf(buffer, "%d - %s", nodo->idEspecialidad, nodo->nombre);
	sprintf(buffer, "%03d | %s", nodo->idEspecialidad, nodo->nombre);

	SendMessage(hLista, LB_ADDSTRING, 0, (LPARAM)buffer);

	recorrerInorden(nodo->derecho, hLista);
}

void llenarListaEspecialidades(HWND hwnd) {
	HWND hLista = GetDlgItem(hwnd, LIST_INFO_PACIENTES);
	if (!hLista) return;

	// Limpia la lista antes de llenar
	SendMessage(hLista, LB_RESETCONTENT, 0, 0);

	recorrerInorden(raiz, hLista);
}

void llenarComboEspecialidades(HWND hwndCombo, Especialidad* nodo) {
	if (nodo == NULL) return;

	// Recorrido in-order (puedes usar pre-order si prefieres otro orden)
	llenarComboEspecialidades(hwndCombo, nodo->izquierdo);

	// Agrega el nombre de la especialidad al ComboBox
	int index = SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)nodo->nombre);

	// Asocia el ID de la especialidad con ese índice
	SendMessage(hwndCombo, CB_SETITEMDATA, index, nodo->idEspecialidad);

	llenarComboEspecialidades(hwndCombo, nodo->derecho);
}


void escribirEspecialidad(Especialidad* nodo, std::ofstream& archivo)
{
	if (nodo != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(nodo), sizeof(Especialidad));
		escribirEspecialidad(nodo->izquierdo, archivo);
		escribirEspecialidad(nodo->derecho, archivo);
	}
}


void leerEspecialidad(std::ifstream& archivo)
{
	while (true)
	{
		Especialidad* nuevo = new Especialidad;
		archivo.read(reinterpret_cast<char*>(nuevo), sizeof(Especialidad));

		if (archivo.eof())
		{
			//delete nuevo;
			break;
		}
		nuevo->derecho = NULL;
		nuevo->izquierdo = NULL;
		agregarEspecialidad(raiz, nuevo);
	}
}