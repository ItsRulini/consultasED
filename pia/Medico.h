#pragma once
#include "Usuario.h"

inline int CompareSystemTime(SYSTEMTIME t1, SYSTEMTIME t2) {
	FILETIME f1, f2;
	SystemTimeToFileTime(&t1, &f1);
	SystemTimeToFileTime(&t2, &f2);
	ULONGLONG u1 = (((ULONGLONG)f1.dwHighDateTime) << 32) + f1.dwLowDateTime;
	ULONGLONG u2 = (((ULONGLONG)f2.dwHighDateTime) << 32) + f2.dwLowDateTime;
	return (u1 < u2) ? -1 : (u1 > u2) ? 1 : 0;
}

struct Medico {
	int cedula; // PK
	char nombre[50];
	char apellidoPaterno[50];
	char apellidoMaterno[50];
	char correo[50];
	long telefono;
	int idEspecialidad; // FK, referencia a la especialidad del médico

	bool estatus;

	Medico* siguiente = NULL;
	Medico* anterior = NULL;
};

Medico* primeroMedico = NULL, * ultimoMedico = NULL;
char archivoMedicos[MAX_PATH];

Medico* crearMedico(int cedula, char nombre[50], char apellidoPaterno[50], char apellidoMaterno[50], char correo[50], long telefono, int idEspecialidad, bool estatus) {
	Medico* nuevoMedico = new Medico;
	nuevoMedico->cedula = cedula;
	strcpy(nuevoMedico->nombre, nombre);
	strcpy(nuevoMedico->apellidoPaterno, apellidoPaterno);
	strcpy(nuevoMedico->apellidoMaterno, apellidoMaterno);
	strcpy(nuevoMedico->correo, correo);
	nuevoMedico->telefono = telefono;
	nuevoMedico->idEspecialidad = idEspecialidad;
	nuevoMedico->estatus = estatus;
	nuevoMedico->siguiente = NULL;
	nuevoMedico->anterior = NULL;
	return nuevoMedico;
}

void agregarMedico(Medico* nuevo) {
	if (primeroMedico == NULL) {
		primeroMedico = nuevo;
		ultimoMedico = nuevo;
	}
	else {
		ultimoMedico->siguiente = nuevo;
		nuevo->anterior = ultimoMedico;
		ultimoMedico = nuevo;
	}
}

Medico* buscarMedico(int cedula) {
	Medico* actual = primeroMedico;
	while (actual != NULL) {
		if (actual->cedula == cedula) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

void medicosPorEspecialidad(int idEspecialidad, HWND hCombo) {
	Medico* actual = primeroMedico;
	while (actual != NULL) {
		if (actual->idEspecialidad == idEspecialidad) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)actual->nombre);
		}
		actual = actual->siguiente;
	}
}

void llenarListaMedicos(HWND hListBox, Medico* cabeza) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0); // Limpiar lista

	for (Medico* actual = cabeza; actual != NULL; actual = actual->siguiente) {
		char linea[128];
		sprintf(linea, "%s %s %s - Cédula: %d", actual->nombre, actual->apellidoPaterno, actual->apellidoMaterno, actual->cedula);

		int index = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)linea);
		SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)actual->cedula);
	}
}


// Método de ordenamiento quicksort para la lista de medicos
void swapMedico(Medico* a, Medico* b)
{
	Medico* temp = new Medico;
	temp->cedula = a->cedula;
	strcpy(temp->nombre, a->nombre);
	strcpy(temp->apellidoPaterno, a->apellidoPaterno);
	strcpy(temp->apellidoMaterno, a->apellidoMaterno);
	strcpy(temp->correo, a->correo);
	temp->telefono = a->telefono;
	temp->idEspecialidad = a->idEspecialidad;
	temp->estatus = a->estatus;

	a->cedula = b->cedula;
	strcpy(a->nombre, b->nombre);
	strcpy(a->apellidoPaterno, b->apellidoPaterno);
	strcpy(a->apellidoMaterno, b->apellidoMaterno);
	strcpy(a->correo, b->correo);
	a->telefono = b->telefono;
	a->idEspecialidad = b->idEspecialidad;
	a->estatus = b->estatus;

	b->cedula = temp->cedula;
	strcpy(b->nombre, temp->nombre);
	strcpy(b->apellidoPaterno, temp->apellidoPaterno);
	strcpy(b->apellidoMaterno, temp->apellidoMaterno);
	strcpy(b->correo, temp->correo);
	b->telefono = temp->telefono;
	b->idEspecialidad = temp->idEspecialidad;
	b->estatus = temp->estatus;

	delete temp;
}

Medico* partitionMedico(Medico* l, Medico* h)
{
	// set pivot as h element
	char x[50];
	char pivotApellidoPaterno[50];
	strcpy(pivotApellidoPaterno, h->apellidoPaterno);
	_strlwr(pivotApellidoPaterno);
	strcpy(x, pivotApellidoPaterno);

	// similar to i = l-1 for array implementation 
	Medico* i = l->anterior;

	// Similar to "for (int j = l; j <= h- 1; j++)" 
	for (Medico* j = l; j != h; j = j->siguiente)
	{
		char apellidoPaternoJ[50];
		strcpy(apellidoPaternoJ, j->apellidoPaterno);
		_strlwr(apellidoPaternoJ);
		if (strcmp(apellidoPaternoJ, x) <= 0)
		{
			// Similar to i++ for array
			i = (i == NULL) ? l : i->siguiente;

			swapMedico((i), (j));
		}
	}
	i = (i == NULL) ? l : i->siguiente; // Similar to i++ 
	swapMedico((i), (h));
	return i;
}


void _quickSortMedico(Medico* l, Medico* h)
{
	if (h != NULL && l != h && l != h->siguiente)
	{
		Medico* p = partitionMedico(l, h);
		_quickSortMedico(l, p->anterior);
		_quickSortMedico(p->siguiente, h);
	}
}

// The main function to sort a linked list. 
// It mainly calls _quickSort() 

void quickSortMedico(Medico* first)
{
	Medico* last = ultimoMedico;
	// Call the recursive QuickSort 
	_quickSortMedico(first, last);
}


void mostrarMedicosPorEspecialidad(HWND hListBox, int especialidad) {

	quickSortMedico(primeroMedico); // Ordenar la lista de médicos antes de mostrar

	// Limpiar la lista antes de mostrar los médicos
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	// Recorrer la lista de médicos y agregar cada uno a la lista
	for (Medico* actual = primeroMedico; actual != NULL; actual = actual->siguiente) {
		if (!actual->estatus || actual->idEspecialidad != especialidad) 
			continue; // Si el médico no está activo o no tiene la especialidad especificada, lo omitimos

		char linea[128];
		sprintf(linea, "%s %s %s - Cédula: %d", actual->nombre, actual->apellidoPaterno, actual->apellidoMaterno, actual->cedula);
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)linea);
	}
	if (SendMessage(hListBox, LB_GETCOUNT, 0, 0) == 0) {
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"No hay médicos disponibles para esta especialidad.");
	}
}



void escribirMedicos(std::ofstream& archivo) {
	Medico* actual = primeroMedico;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Medico));
		actual = actual->siguiente;
	}
}

void leerMedicos(std::ifstream& archivo) {
	while (true)
	{
		Medico* nuevo = new Medico;
		archivo.read(reinterpret_cast<char*>(nuevo), sizeof(Medico));

		if (archivo.eof())
		{
			delete nuevo; // libreando la memoria que creamos y no utilizamos del puntero 'nuevo'
			break;
		}

		agregarMedico(nuevo);
	}
}