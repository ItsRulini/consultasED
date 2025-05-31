#pragma once

#include "Consultorio.h"

enum ESTADO_CITA {
	PENDIENTE = 0,
	CANCELADA = 1,
	REALIZADA = 2
};

struct Cita {
	int cedulaMedico; // PK
	int idPaciente; // PK
	char diagnostico[MAX_PATH];
	SYSTEMTIME fechaHoraCita;
	ESTADO_CITA estatus;
	int idConsultorio; // FK

	Cita* siguiente = NULL;
	Cita* anterior = NULL;
};

Cita* primeraCita = NULL, * ultimaCita = NULL;
char archivoCitas[MAX_PATH];

Cita* crearCita(int cedulaMedico, int idPaciente, int idConsultorio, char diagnostico[MAX_PATH], SYSTEMTIME fechaHoraCita, ESTADO_CITA estatus) {
	Cita* nuevaCita = new Cita;
	nuevaCita->cedulaMedico = cedulaMedico;
	nuevaCita->idPaciente = idPaciente;
	strcpy(nuevaCita->diagnostico, diagnostico);
	nuevaCita->fechaHoraCita = fechaHoraCita;
	nuevaCita->estatus = estatus;
	nuevaCita->idConsultorio = idConsultorio;
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

Cita* buscarCitaEspecifica(int cedulaMedico, int idPaciente) {
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
		agregarCita(actual);
		actual = new Cita;
	}
}