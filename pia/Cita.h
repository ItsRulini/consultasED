#pragma once

#include "Consultorio.h"

enum ESTADO_CITA {
	PENDIENTE = 0,
	CANCELADA = 1,
	REALIZADA = 2
};

struct Cita {
	int idCita; // PK, autoincremental
	
	char diagnostico[MAX_PATH];
	SYSTEMTIME fechaHoraCita;
	ESTADO_CITA estatus;
	int idConsultorio; // FK, a Consultorio
	int cedulaMedico; // FK a Medico
	int idPaciente; // FK a Paciente

	Cita* siguiente = NULL;
	Cita* anterior = NULL;
};

Cita* primeraCita = NULL, * ultimaCita = NULL;
char archivoCitas[MAX_PATH];

int generarIdCita();
void validarFechaHoraCita(Cita*);

Cita* crearCita(int cedulaMedico, int idPaciente, int idConsultorio, char diagnostico[MAX_PATH], SYSTEMTIME fechaHoraCita, ESTADO_CITA estatus) {
	Cita* nuevaCita = new Cita;
	nuevaCita->idCita = generarIdCita(); // Genera un ID único para la cita
	nuevaCita->idConsultorio = idConsultorio;
	nuevaCita->cedulaMedico = cedulaMedico;
	nuevaCita->idPaciente = idPaciente;
	strcpy(nuevaCita->diagnostico, diagnostico);
	nuevaCita->fechaHoraCita = fechaHoraCita;
	nuevaCita->estatus = estatus;
	nuevaCita->siguiente = NULL;
	nuevaCita->anterior = NULL;
	return nuevaCita;
}

void agregarCita(Cita* nueva) {
	if (primeraCita == NULL) {
		primeraCita = nueva;
		ultimaCita = nueva;
	}
	else {
		ultimaCita->siguiente = nueva;
		nueva->anterior = ultimaCita;
		ultimaCita = nueva;
	}
}

Cita* buscarCitaPorId(int idCita) {
	Cita* actual = primeraCita;
	while (actual != NULL) {
		if (actual->idCita == idCita) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Cita* buscarCitaMedicoPaciente(int cedulaMedico, int idPaciente) {
	Cita* actual = primeraCita;
	while (actual != NULL) {
		if (actual->cedulaMedico == cedulaMedico && actual->idPaciente == idPaciente) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Cita* buscarCitaPorPaciente(int idPaciente) {
	Cita* actual = primeraCita;
	while (actual != NULL) {
		if (actual->idPaciente == idPaciente) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Cita* buscarCitaPorMedico(int cedulaMedico) {
	Cita* actual = primeraCita;
	while (actual != NULL) {
		if (actual->cedulaMedico == cedulaMedico) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}


// Función que convierte SYSTEMTIME a segundos desde 01/01/1970 (Epoch)
long long convertirAsegundos(const SYSTEMTIME& fecha) {
	FILETIME ft;
	SystemTimeToFileTime(&fecha, &ft); // Convierte SYSTEMTIME a FILETIME
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return ull.QuadPart / 10000000LL - 11644473600LL; // Convierte a segundos desde el Epoch
}

// Método de ordenamiento heapsort para ordenar a las citas de más reciente a más antigua (descendente)

Cita* obtenerUltimaCita()
{
	Cita* temporal = primeraCita;
	while (temporal->siguiente)
		temporal = temporal->siguiente;
	return temporal;
}

Cita* HeapInsert(Cita* pHead, Cita* pNew)
{
	if (pHead == NULL)
	{
		pNew->anterior = pNew->siguiente = NULL;
		return pNew;
	}

	long long segundosNew = convertirAsegundos(pNew->fechaHoraCita);
	long long segundosHead = convertirAsegundos(pHead->fechaHoraCita);

	if (segundosNew > segundosHead)
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

Cita* HeapRemove(Cita* pHead)
{
	if (pHead == NULL)
		return NULL;

	if (pHead->anterior == NULL)
		return pHead->siguiente;
	if (pHead->siguiente == NULL)
		return pHead->anterior;

	long long segundosAnterior = convertirAsegundos(pHead->anterior->fechaHoraCita);
	long long segundosSiguiente = convertirAsegundos(pHead->siguiente->fechaHoraCita);

	if (segundosAnterior > segundosSiguiente)
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

Cita* HeapSort(Cita* pList)
{
	Cita* pIter = pList, * pNext, * pPrev;
	Cita* pHead = NULL;
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

// Funciones de médicos
void mostrarListaCitasMedico(HWND hListBox, int cedulaMedico) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

	Cita* primero = primeraCita;
	Cita* aux = primeraCita = HeapSort(primero); // Ordenando antes de mostrar para mas eficiencia
	ultimaCita = obtenerUltimaCita();

	Cita* actual = primeraCita;
	while (actual != nullptr) {
		if (actual->cedulaMedico == cedulaMedico) {
			// Buscar consultorio
			Consultorio* consultorio = buscarConsultorio(primeroConsultorio, actual->idConsultorio);
			Medico* medico = buscarMedico(actual->cedulaMedico);
			Especialidad* especialidad = NULL;
			if(medico!= NULL)
				especialidad = buscarEspecialidad(raiz, medico->idEspecialidad);

			const char* esp = especialidad ? especialidad->nombre : "Desconocida";

			validarFechaHoraCita(actual); // Validar si la cita ya pasó y actualizar su estatus si es necesario

			// Obtener estatus como texto
			const char* estatusStr = "";
			switch (actual->estatus) {
			case PENDIENTE: estatusStr = "Pendiente"; break;
			case CANCELADA: estatusStr = "Cancelada"; break;
			case REALIZADA: estatusStr = "Realizada"; break;
			default: estatusStr = "Desconocido"; break;
			}

			// Formatear la cadena
			char buffer[200];
			sprintf_s(buffer, " %d | %d | %s | %s", actual->idCita, actual->idConsultorio, esp, estatusStr);
			int index = SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
			// Asignar el ID de la cita como dato del ítem
			SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)actual->idCita);
		}
		actual = actual->siguiente;
	}

	if(SendMessage(hListBox, LB_GETCOUNT, 0, 0) == 0) {
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)"No hay citas para este médico.");
	}
}

void mostrarListaCitasMedicoFiltradoPorFechas(HWND hListBox, int cedulaMedico, long long segundosInicio, long long segundosFin) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

	Cita* primero = primeraCita;
	Cita* aux = primeraCita = HeapSort(primero); // Ordenar antes de mostrar para más eficiencia
	ultimaCita = obtenerUltimaCita();

	Cita* actual = primeraCita;
	while (actual != nullptr) {
		if (actual->cedulaMedico == cedulaMedico) {
			long long segundosCita = convertirAsegundos(actual->fechaHoraCita);
			if (segundosCita >= segundosInicio && segundosCita <= segundosFin) {
				// Buscar consultorio
				Consultorio* consultorio = buscarConsultorio(primeroConsultorio, actual->idConsultorio);
				Medico* medico = buscarMedico(actual->cedulaMedico);
				Especialidad* especialidad = NULL;
				if (medico != NULL)
					especialidad = buscarEspecialidad(raiz, medico->idEspecialidad);

				const char* esp = especialidad ? especialidad->nombre : "Desconocida";

				validarFechaHoraCita(actual); // Validar si la cita ya pasó y actualizar su estatus si es necesario

				// Obtener estatus como texto
				const char* estatusStr = "";
				switch (actual->estatus) {
				case PENDIENTE: estatusStr = "Pendiente"; break;
				case CANCELADA: estatusStr = "Cancelada"; break;
				case REALIZADA: estatusStr = "Realizada"; break;
				default: estatusStr = "Desconocido"; break;
				}

				// Formatear la cadena
				char buffer[200];
				sprintf_s(buffer, " %d | %d | %s | %s", actual->idCita, actual->idConsultorio, esp, estatusStr);
				int index = SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
				// Asignar el ID de la cita como dato del ítem
				SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)actual->idCita);
			}
		}
		actual = actual->siguiente;
	}

	if (SendMessage(hListBox, LB_GETCOUNT, 0, 0) == 0) {
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)"No hay citas para este médico en el rango de fechas.");
	}
}

// Funciones de pacientes
void mostrarListaCitasPaciente(HWND hListBox, int idPaciente) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

	Cita* primero = primeraCita;
	Cita* aux = primeraCita = HeapSort(primero); // Ordenando antes de mostrar para mas eficiencia
	ultimaCita = obtenerUltimaCita();

	Cita* actual = primeraCita;
	while (actual != nullptr) {
		if (actual->idPaciente == idPaciente) {
			// Buscar consultorio
			Consultorio* consultorio = buscarConsultorio(primeroConsultorio, actual->idConsultorio);
			Medico* medico = buscarMedico(actual->cedulaMedico);
			Especialidad* especialidad = NULL;
			if (medico != NULL)
				especialidad = buscarEspecialidad(raiz, medico->idEspecialidad);

			const char* esp = especialidad ? especialidad->nombre : "Desconocida";

			validarFechaHoraCita(actual); // Validar si la cita ya pasó y actualizar su estatus si es necesario

			// Obtener estatus como texto
			const char* estatusStr = "";
			switch (actual->estatus) {
			case PENDIENTE: estatusStr = "Pendiente"; break;
			case CANCELADA: estatusStr = "Cancelada"; break;
			case REALIZADA: estatusStr = "Realizada"; break;
			default: estatusStr = "Desconocido"; break;
			}

			// Formatear la cadena
			char buffer[200];
			sprintf_s(buffer, " %d | %d | %s | %s", actual->idCita, actual->idConsultorio, esp, estatusStr);
			int index = SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
			// Asignar el ID de la cita como dato del ítem
			SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)actual->idCita);
		}
		actual = actual->siguiente;
	}

	if (SendMessage(hListBox, LB_GETCOUNT, 0, 0) == 0) {
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)"No hay citas para este paciente.");
	}
}

void mostrarListaCitasPacienteFiltradoPorFechas(HWND hListBox, int idPaciente, long long segundosInicio, long long segundosFin) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

	Cita* primero = primeraCita;
	Cita* aux = primeraCita = HeapSort(primero); // Ordenar antes de mostrar para más eficiencia
	ultimaCita = obtenerUltimaCita();

	Cita* actual = primeraCita;
	while (actual != nullptr) {
		if (actual->idPaciente == idPaciente) {
			long long segundosCita = convertirAsegundos(actual->fechaHoraCita);
			if (segundosCita >= segundosInicio && segundosCita <= segundosFin) {
				// Buscar consultorio
				Consultorio* consultorio = buscarConsultorio(primeroConsultorio, actual->idConsultorio);
				Medico* medico = buscarMedico(actual->cedulaMedico);
				Especialidad* especialidad = NULL;
				if (medico != NULL)
					especialidad = buscarEspecialidad(raiz, medico->idEspecialidad);

				const char* esp = especialidad ? especialidad->nombre : "Desconocida";

				validarFechaHoraCita(actual); // Validar si la cita ya pasó y actualizar su estatus si es necesario

				// Obtener estatus como texto
				const char* estatusStr = "";
				switch (actual->estatus) {
				case PENDIENTE: estatusStr = "Pendiente"; break;
				case CANCELADA: estatusStr = "Cancelada"; break;
				case REALIZADA: estatusStr = "Realizada"; break;
				default: estatusStr = "Desconocido"; break;
				}

				// Formatear la cadena
				char buffer[200];
				sprintf_s(buffer, " %d | %d | %s | %s", actual->idCita, actual->idConsultorio, esp, estatusStr);
				int index = SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
				// Asignar el ID de la cita como dato del ítem
				SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)actual->idCita);
			}
		}
		actual = actual->siguiente;
	}

	if (SendMessage(hListBox, LB_GETCOUNT, 0, 0) == 0) {
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)"No hay citas para este paciente en el rango de fechas.");
	}
}


// Helper function to add seconds to a SYSTEMTIME structure
void AddSecondsToSystemTime(SYSTEMTIME* st, int seconds) {
	FILETIME ft;
	ULARGE_INTEGER uli;

	SystemTimeToFileTime(st, &ft);

	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;

	// Add seconds (1 second = 10,000,000 100-nanosecond intervals)
	uli.QuadPart += (ULONGLONG)seconds * 10000000;

	FileTimeToSystemTime((const FILETIME*)&uli, st);
}

int generarIdCita() {
	static int idCita = 0;
	return ++idCita; // Incrementa el ID cada vez que se llama
}


void actualizarEstatusCita(Cita* cita, ESTADO_CITA nuevoEstatus) {
	if (cita != NULL) {
		cita->estatus = nuevoEstatus;
	}
}

void validarFechaHoraCita(Cita* cita) {
	SYSTEMTIME tiempoActual;
	GetLocalTime(&tiempoActual); // Obtener la fecha y hora actuales del sistema.
	// Comparamos si la hora de la cita es anterior a la hora actual.
	if (CompareSystemTime(tiempoActual, cita->fechaHoraCita) > 0) {
		actualizarEstatusCita(cita, REALIZADA); // Cambia el estatus de la cita a REALIZADA si la fecha ya pasó.
	}
}

void escribirCitas(std::ofstream& archivo) {
	Cita* actual = primeraCita;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Cita));
		actual = actual->siguiente;
	}
}

void leerCitas(std::ifstream& archivo) {
	Cita* actual = new Cita;
	while (archivo.read(reinterpret_cast<char*>(actual), sizeof(Cita))) {
		
		// Validar si la cita está pendiente y si su fecha ya pasó para actualizar su estatus.
		if (actual->estatus == PENDIENTE) {
			validarFechaHoraCita(actual);
		}
		agregarCita(actual);
		generarIdCita(); // Asegura que el ID se genere correctamente
		actual = new Cita;
	}

	delete actual; // Liberar memoria del último nodo
}