#pragma once

#include "Medico.h"

enum ESTADO_CONSULTORIO {
	DISPONIBLE = 0,
	OCUPADO = 1
};

struct Consultorio {
	int idConsultorio;
	ESTADO_CONSULTORIO estatus;

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

Consultorio* buscarConsultorio(int idConsultorio) {
	Consultorio* actual = primeroConsultorio;
	while (actual != NULL) {
		if (actual->idConsultorio == idConsultorio) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
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