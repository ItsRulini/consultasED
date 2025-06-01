#pragma once

#include "Medico.h"

enum ESTADO_CONSULTORIO {
	DISPONIBLE = 0,
	OCUPADO = 1
};

struct Consultorio {
	int idConsultorio;
	ESTADO_CONSULTORIO estatus;
	int cedulaMedico; // FK a Medico

	Consultorio* siguiente = NULL;
	Consultorio* anterior = NULL;
};

Consultorio* primeroConsultorio = NULL, * ultimoConsultorio = NULL;
char archivoConsultorios[MAX_PATH];

Consultorio* crearConsultorio(int idConsultorio, ESTADO_CONSULTORIO estatus) {
	Consultorio* nuevoConsultorio = new Consultorio;
	nuevoConsultorio->idConsultorio = idConsultorio;
	nuevoConsultorio->estatus = estatus;
	nuevoConsultorio->siguiente = NULL;
	nuevoConsultorio->anterior = NULL;
	return nuevoConsultorio;
}

void agregarConsultorio(Consultorio* nuevo) {

	if (primeroConsultorio == NULL) {
		primeroConsultorio = nuevo;
		ultimoConsultorio = nuevo;
	}
	else {
		ultimoConsultorio->siguiente = nuevo;
		nuevo->anterior = ultimoConsultorio;
		ultimoConsultorio = nuevo;
	}
}

// Búsqueda binaria para la lista de consultorios
Consultorio* middle(Consultorio* start, Consultorio* last)
{
	if (start == NULL)
		return NULL;
	Consultorio* slow = start;
	Consultorio* fast = start->siguiente;
	while (fast != last)
	{
		fast = fast->siguiente;
		if (fast != last) {
			slow = slow->siguiente;
			fast = fast->siguiente;
		}
	}
	return slow;
}

Consultorio* buscarConsultorio(Consultorio* head, int codigo)
{
	Consultorio* start = head;
	Consultorio* last = NULL;
	do {
		Consultorio* mid = middle(start, last); // Find middle
		if (mid == NULL) // If middle is empty
			return NULL;
		if (mid->idConsultorio == codigo) // If value is present at middle
			return mid;
		else if (mid->idConsultorio < codigo) // If value is more than mid
			start = mid->siguiente;
		else // If the value is less than mid.
			last = mid;
	} while (last == NULL || last != start);
	return NULL; // value not present
}


// Método de ordenamiento quicksort para la lista de consultorios y que funcione la busqueda binaria
void swapConsultorio(Consultorio* a, Consultorio* b)
{
	Consultorio* temp = new Consultorio;
	temp->idConsultorio = a->idConsultorio;
	temp->estatus = a->estatus;
	temp->cedulaMedico = a->cedulaMedico;

	a->idConsultorio = b->idConsultorio;
	a->estatus = b->estatus;
	a->cedulaMedico = b->cedulaMedico;

	b->idConsultorio = temp->idConsultorio;
	b->estatus = temp->estatus;
	b->cedulaMedico = temp->cedulaMedico;

	delete temp;
}

Consultorio* partitionConsultorio(Consultorio* l, Consultorio* h)
{
	// set pivot as h element
	int x = h->idConsultorio;

	// similar to i = l-1 for array implementation 
	Consultorio* i = l->anterior;

	// Similar to "for (int j = l; j <= h- 1; j++)" 
	for (Consultorio* j = l; j != h; j = j->siguiente)
	{
		if (j->idConsultorio <= x)
		{
			// Similar to i++ for array
			i = (i == NULL) ? l : i->siguiente;

			swapConsultorio((i), (j));
		}
	}
	i = (i == NULL) ? l : i->siguiente; // Similar to i++ 
	swapConsultorio((i), (h));
	return i;
}


void _quickSortConsultorio(Consultorio* l, Consultorio* h)
{
	if (h != NULL && l != h && l != h->siguiente)
	{
		Consultorio* p = partitionConsultorio(l, h);
		_quickSortConsultorio(l, p->anterior);
		_quickSortConsultorio(p->siguiente, h);
	}
}

// The main function to sort a linked list. 
// It mainly calls _quickSort() 

void quickSortConsultorio(Consultorio* first)
{
	Consultorio* last = ultimoConsultorio;
	// Call the recursive QuickSort 
	_quickSortConsultorio(first, last);
}


void escribirConsultorios(std::ofstream& archivo) {
	Consultorio* actual = primeroConsultorio;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Consultorio));
		actual = actual->siguiente;
	}
}

void leerConsultorios(std::ifstream& archivo) {
	Consultorio* nuevoConsultorio;
	while (!archivo.eof()) {
		nuevoConsultorio = new Consultorio;
		archivo.read(reinterpret_cast<char*>(nuevoConsultorio), sizeof(Consultorio));
		if (archivo.eof()) {
			delete nuevoConsultorio;
			break;
		}
		agregarConsultorio(nuevoConsultorio);
	}
}