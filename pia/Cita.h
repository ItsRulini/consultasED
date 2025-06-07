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