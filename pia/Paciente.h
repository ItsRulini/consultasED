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
Paciente* auxPaciente = NULL;
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

int calcularEdad(Paciente* paciente) {
	SYSTEMTIME fechaActual;
	GetLocalTime(&fechaActual);
	int edad = fechaActual.wYear - paciente->fechaNacimiento.wYear;
	if (fechaActual.wMonth < paciente->fechaNacimiento.wMonth ||
		(fechaActual.wMonth == paciente->fechaNacimiento.wMonth && fechaActual.wDay < paciente->fechaNacimiento.wDay)) {
		edad--;
	}
	return edad;
}

void llenarInfoPaciente(HWND hDlg, Paciente* paciente) {
	SetDlgItemTextA(hDlg, NOMBRE_AGENDAR, paciente->nombre);
	SetDlgItemTextA(hDlg, PATERNO_AGENDAR, paciente->apellidoPaterno);
	SetDlgItemTextA(hDlg, MATERNO_AGENDAR, paciente->apellidoMaterno);
	SetDlgItemInt(hDlg, EDAD_AGENDAR, calcularEdad(paciente), FALSE);
	
	// Seleccionar el radio button según el sexo
	if (paciente->sexo == MASCULINO) {
		CheckDlgButton(hDlg, HOMBRE_AGENDAR, BST_CHECKED);
		CheckDlgButton(hDlg, MUJER_AGENDAR, BST_UNCHECKED);
	}
	else {
		CheckDlgButton(hDlg, MUJER_AGENDAR, BST_CHECKED);
		CheckDlgButton(hDlg, HOMBRE_AGENDAR, BST_UNCHECKED);
	}
}

// Método de ordenamiento quicksort para la lista de medicos
void swapPaciente(Paciente* a, Paciente* b)
{
	Paciente* temp = new Paciente;
	temp->idPaciente = a->idPaciente;
	strcpy(temp->nombre, a->nombre);
	strcpy(temp->apellidoPaterno, a->apellidoPaterno);
	strcpy(temp->apellidoMaterno, a->apellidoMaterno);
	strcpy(temp->correo, a->correo);
	temp->telefono = a->telefono;
	temp->fechaNacimiento = a->fechaNacimiento;
	temp->sexo = a->sexo;
	temp->estatus = a->estatus;

	a->idPaciente = b->idPaciente;
	strcpy(a->nombre, b->nombre);
	strcpy(a->apellidoPaterno, b->apellidoPaterno);
	strcpy(a->apellidoMaterno, b->apellidoMaterno);
	strcpy(a->correo, b->correo);
	a->telefono = b->telefono;
	a->fechaNacimiento = b->fechaNacimiento;
	a->sexo = b->sexo;
	a->estatus = b->estatus;

	b->idPaciente = temp->idPaciente;
	strcpy(b->nombre, temp->nombre);
	strcpy(b->apellidoPaterno, temp->apellidoPaterno);
	strcpy(b->apellidoMaterno, temp->apellidoMaterno);
	strcpy(b->correo, temp->correo);
	b->telefono = temp->telefono;
	b->fechaNacimiento = temp->fechaNacimiento;
	b->sexo = temp->sexo;
	b->estatus = temp->estatus;

	delete temp;
}

Paciente* partitionPaciente(Paciente* l, Paciente* h)
{
	// set pivot as h element
	char x[50];
	char pivotApellidoPaterno[50];
	strcpy(pivotApellidoPaterno, h->apellidoPaterno);
	_strlwr(pivotApellidoPaterno);
	strcpy(x, pivotApellidoPaterno);

	// similar to i = l-1 for array implementation 
	Paciente* i = l->anterior;

	// Similar to "for (int j = l; j <= h- 1; j++)" 
	for (Paciente* j = l; j != h; j = j->siguiente)
	{
		char apellidoPaternoJ[50];
		strcpy(apellidoPaternoJ, j->apellidoPaterno);
		_strlwr(apellidoPaternoJ);
		if (strcmp(apellidoPaternoJ, x) <= 0)
		{
			// Similar to i++ for array
			i = (i == NULL) ? l : i->siguiente;

			swapPaciente((i), (j));
		}
	}
	i = (i == NULL) ? l : i->siguiente; // Similar to i++ 
	swapPaciente((i), (h));
	return i;
}


void _quickSortPaciente(Paciente* l, Paciente* h)
{
	if (h != NULL && l != h && l != h->siguiente)
	{
		Paciente* p = partitionPaciente(l, h);
		_quickSortPaciente(l, p->anterior);
		_quickSortPaciente(p->siguiente, h);
	}
}

// The main function to sort a linked list. 
// It mainly calls _quickSort() 

void quickSortPaciente(Paciente* first)
{
	Paciente* last = ultimoPaciente;
	// Call the recursive QuickSort 
	_quickSortPaciente(first, last);
}



void escribirPacientes(std::ofstream& archivo) {
	Paciente* actual = primeroPaciente;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Paciente));
		actual = actual->siguiente;
	}
}

void llenarListaPacientes(HWND hListBox, Paciente* lista, boolean filtro) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	while (lista != NULL) {

		if (filtro != NULL) { // es diferente de NULL, por lo tanto se filtra
			if (filtro == true) {
				// Mostrar solo pacientes activos
				if (lista->estatus == false) {
					lista = lista->siguiente;
					continue; // Si el paciente no está activo, lo saltamos
				}
			}
			else {
				// Mostrar solo pacientes inactivos
				if (lista->estatus == true) {
					lista = lista->siguiente;
					continue; // Si el paciente está activo, lo saltamos
				}
			}
		}

		char estatus[30];
		if (lista->estatus) {
			strcpy(estatus, "Activo");
		}
		else {
			strcpy(estatus, "Inactivo");
		}

		// Si es NULL, no se filtra, se muestran todos los pacientes
		char buffer[150];
		sprintf_s(buffer, "%d | %s %s %s | %s", lista->idPaciente, lista->nombre, lista->apellidoPaterno, lista->apellidoMaterno, estatus);
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