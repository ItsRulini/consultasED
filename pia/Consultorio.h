#pragma once

#include "Especialidad.h" // Tiene médico y especialidad

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
	nuevoConsultorio->cedulaMedico = 0; // Se asignará después
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


Consultorio* buscarConsultorioPorMedico(int cedulaMedico) {
	Consultorio* actual = primeroConsultorio;
	while (actual != NULL) {
		if (actual->cedulaMedico == cedulaMedico) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL; // No se encontró el consultorio
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

void llenarEspecialidadDelMedico(HWND hwnd, int cedulaMedico) {
	Medico* medico = buscarMedico(cedulaMedico);
	if (medico != NULL) {
		// Buscar la especialidad del médico
		Especialidad* esp = buscarEspecialidad(raiz, medico->idEspecialidad);
		if (esp != NULL) {
			// Llenar el campo de especialidad
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, esp->nombre);
		}
		else {
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "Especialidad no encontrada");
		}
	}
	else {
		SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");
	}
}

// ===== FUNCIÓN PARA EXTRAER CÉDULA DEL COMBO =====

int obtenerCedulaDelCombo(HWND hwnd) {
	HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
	int seleccion = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

	if (seleccion != CB_ERR) {
		char buffer[100];
		SendMessageA(hCombo, CB_GETLBTEXT, seleccion, (LPARAM)buffer);

		// Extraer la cédula (formato: "12345 - Nombre Apellido1 Apellido2")
		int cedula = 0;
		if (sscanf(buffer, "%d", &cedula) == 1) {
			return cedula;
		}
	}
	return -1; // Error o no hay selección
}

void llenarComboMedicosEspecialidadConsultorio(HWND hwnd, int especilidad) {
	HWND hComboBox = GetDlgItem(hwnd, COMBO_MEDICOS);
	SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);
	Medico* lista = primeroMedico;
	while (lista != NULL) {
		// Solo mostrar médicos activos y de la especialidad del consultorio
		if (lista->estatus != true || lista->idEspecialidad != especilidad) {
			lista = lista->siguiente;
			continue; // Saltar médicos inactivos
		}

		char buffer[100];
		sprintf_s(buffer, "%d - %s %s %s", lista->cedula, lista->nombre, lista->apellidoPaterno, lista->apellidoMaterno);
		SendMessageA(hComboBox, CB_ADDSTRING, 0, (LPARAM)buffer);
		lista = lista->siguiente;
	}
}

void llenarComboMedicos(HWND hwnd) {
	HWND hComboBox = GetDlgItem(hwnd, COMBO_MEDICOS);
	SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);
	Medico* lista = primeroMedico;
	while (lista != NULL) {
		// Solo mostrar médicos activos
		if(lista->estatus != true) {
			lista = lista->siguiente;
			continue; // Saltar médicos inactivos
		}
		
		char buffer[100];
		sprintf_s(buffer, "%d - %s %s %s", lista->cedula, lista->nombre, lista->apellidoPaterno, lista->apellidoMaterno);
		SendMessageA(hComboBox, CB_ADDSTRING, 0, (LPARAM)buffer);
		lista = lista->siguiente;
	}
}


void llenarListaConsultorios(HWND hwnd) {
	// Ordenar la lista de consultorios antes de llenar la lista
	quickSortConsultorio(primeroConsultorio);
	Consultorio* lista = primeroConsultorio;

	HWND hListBox = GetDlgItem(hwnd, LIST_CONSULTORIOS);
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	while (lista != NULL) {
		char buffer[150];
		Medico* medico = buscarMedico(lista->cedulaMedico);
		char nombreMedico[50] = "Desconocido";
		char especialidad[50] = "Desconocida";
		// Concatenar nombre del médico y especialidad
		if (medico != NULL) {
			strcpy(nombreMedico, medico->nombre);
			strcat(nombreMedico, " ");
			strcat(nombreMedico, medico ? medico->apellidoPaterno : "Desconocido");
			strcat(nombreMedico, " ");
			strcat(nombreMedico, medico ? medico->apellidoMaterno : "Desconocido");
			strcat(nombreMedico, " - ");
			Especialidad* esp = buscarEspecialidad(raiz, medico->idEspecialidad);
			strcpy(especialidad, esp ? esp->nombre : "Desconocida");
		}
		
		sprintf_s(buffer, "%d - %s -> %s", lista->idConsultorio, nombreMedico, especialidad);
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
		lista = lista->siguiente;
	}
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