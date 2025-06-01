#include "Medico.h"
#include "Paciente.h"
#include "Especialidad.h"
#include "Consultorio.h"
#include "Cita.h"

// Declarar el prototipo de la función ventanaInicio
LRESULT CALLBACK vInicio(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vDoctor(HWND, UINT, WPARAM, LPARAM); 
LRESULT CALLBACK vPaciente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vConsultorio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);

// Archivos binarios
void InicializarArchivos();
void leerArchivosBinarios();
void guardarArchivosBinarios();

// Menú
void menu(HWND hwnd, WPARAM wParam);

// Funciones para llenar combo box
int CompareSystemTime(SYSTEMTIME t1, SYSTEMTIME t2);
//void llenarComboEspecialidades(HWND hwndCombo, Especialidad* nodo);

// Variables globales
bool registro = false;
bool modificar = false;

HINSTANCE hInstGlobal;

int WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int cShow) {
	InicializarArchivos();
	leerArchivosBinarios();
	/*HWND hVentanaInicial = */
	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
	MSG msg; //Variable para los mensajes recibidos en la ventana
	ZeroMemory(&msg, sizeof(MSG));

	//ShowWindow(hVentanaInicial, cShow);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK vInicio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
	case WM_COMMAND: {

		switch (LOWORD(wParam)) {
		case INGRESAR_BTN: {

			HWND hUsuario = GetDlgItem(hwnd, IDC_USUARIO_ADMIN);
			HWND hContrasena = GetDlgItem(hwnd, IDC_PASS_ADMIN);
			char usuario[50];
			char contrasena[50];

			GetWindowText(hUsuario, usuario, sizeof(usuario));
			GetWindowText(hContrasena, contrasena, sizeof(contrasena));

			if (strcmp(usuario, "admin") == 0 && strcmp(contrasena, "admin") == 0) {
				EndDialog(hwnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
			}
			else {
				MessageBox(NULL, "Usuario o contraseña incorrectos", "Error", MB_OK | MB_ICONERROR);
			}

		} break;

		}

	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}

	return FALSE;
}


LRESULT CALLBACK vDoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Medico* medicoSeleccionado = NULL;

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		HWND hComboEspecialidad = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
		SendMessage(hComboEspecialidad, CB_RESETCONTENT, 0, 0); // Limpia por si acaso
		llenarComboEspecialidades(hComboEspecialidad, raiz); // Llenar desde la raíz del árbol
		HWND hListMedicos = GetDlgItem(hwnd, LIST_INFO_MEDICOS);
		llenarListaMedicos(hListMedicos, primeroMedico); // donde cabezaMedicos es el inicio de la lista enlazada
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
		case APLICAR_MEDICO_BTN: {
			HWND hNombreMedico = GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO);
			HWND hPaternoMedico = GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO);
			HWND hMaternoMedico = GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO);
			HWND hCedula = GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO);
			HWND hEmail = GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO);
			HWND hTelefono = GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO);
			HWND hEspecialidad = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
			HWND hActivo = GetDlgItem(hwnd, IDC_ACTIVO_MEDICO);
			HWND hInactivo = GetDlgItem(hwnd, IDC_INACTIVO_MEDICO);

			char nombre[50], paterno[50], materno[50], correo[50], buffer[20];
			int cedula = 0;
			long telefono = 0;

			// Obtener textos
			GetWindowTextA(hNombreMedico, nombre, 50);
			GetWindowTextA(hPaternoMedico, paterno, 50);
			GetWindowTextA(hMaternoMedico, materno, 50);
			GetWindowTextA(hCedula, buffer, 20);

			if (strlen(nombre) == 0) {
				MessageBox(hwnd, "El nombre del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (strlen(paterno) == 0) {
				MessageBox(hwnd, "El apellido paterno del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (strlen(materno) == 0) {
				MessageBox(hwnd, "El apellido materno del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que la cédula no esté vacía
			if (strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "La cédula no puede estar vacía.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &cedula) != 1 && !modificar) {
				MessageBox(hwnd, "Cédula inválida. Debe ser un número entero.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que el teléfono no esté vacío

			GetWindowTextA(hTelefono, buffer, 20);
			if (strlen(buffer) == 0) {
				MessageBox(hwnd, "El teléfono no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			// Validar que el correo no esté vacío
			GetWindowTextA(hEmail, correo, 50);
			if (strlen(correo) == 0) {
				MessageBox(hwnd, "El correo no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			GetWindowTextA(hTelefono, buffer, 20);
			if (sscanf(buffer, "%ld", &telefono) != 1) {
				MessageBox(hwnd, "Teléfono inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			GetWindowTextA(hEmail, correo, 50);

			// Validar selección en el ComboBox de especialidad
			int index = SendMessage(hEspecialidad, CB_GETCURSEL, 0, 0);
			

			if (index == CB_ERR) {
				MessageBox(hwnd, "Seleccione una especialidad.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			// Obtener el ID de la especialidad (puede ser el mismo índice o asociado con CB_GETITEMDATA si lo manejas)
			//int idEspecialidad = index; // ajusta si usas CB_SETITEMDATA / CB_GETITEMDATA
			int idEspecialidad = SendMessage(hEspecialidad, CB_GETITEMDATA, index, 0);

			// Validar estatus (activo o inactivo)
			bool estatus = false;
			if (SendMessage(hActivo, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				estatus = true;
			}
			else if (SendMessage(hInactivo, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				estatus = false;
			}
			else {
				MessageBox(hwnd, "Seleccione un estatus para el médico (Activo/Inactivo).", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Verificar si ya existe un médico con la misma cédula
			if (buscarMedico(cedula) != NULL) {
				MessageBox(hwnd, "Ya existe un médico con esta cédula.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (medicoSeleccionado != NULL) {
				// Modificar médico existente
				strcpy(medicoSeleccionado->nombre, nombre);
				strcpy(medicoSeleccionado->apellidoPaterno, paterno);
				strcpy(medicoSeleccionado->apellidoMaterno, materno);
				strcpy(medicoSeleccionado->correo, correo);
				medicoSeleccionado->telefono = telefono;
				medicoSeleccionado->idEspecialidad = idEspecialidad;
				medicoSeleccionado->estatus = estatus;

				MessageBox(hwnd, "Médico modificado correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				// Validar que no exista un médico con ese ID
				if (buscarMedico(cedula)) {
					MessageBox(hwnd, "Ya existe un médico con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				// Crear y agregar médico a la lista
				Medico* nuevo = crearMedico(cedula, nombre, paterno, materno, correo, telefono, idEspecialidad, estatus);
				agregarMedico(nuevo);
				MessageBox(hwnd, "Médico agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			// Crear y agregar médico a la lista
			/*Medico* nuevo = crearMedico(cedula, nombre, paterno, materno, correo, telefono, idEspecialidad, estatus);
			agregarMedico(nuevo);*/

			llenarListaMedicos(GetDlgItem(hwnd, LIST_INFO_MEDICOS), primeroMedico);

		} break;
		case BUSCAR_MEDICO_BTN: {
			HWND hCedula = GetDlgItem(hwnd, IDC_BUSCAR_MEDICO);
			char buffer[20];
			GetWindowTextA(hCedula, buffer, 20);

			if (strlen(buffer) == 0) {
				MessageBox(hwnd, "Ingrese la cédula del médico a buscar.", "Advertencia", MB_OK | MB_ICONWARNING);
				break;
			}

			int cedula = 0;
			if (sscanf(buffer, "%d", &cedula) != 1) {
				MessageBox(hwnd, "Cédula inválida.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			medicoSeleccionado = buscarMedico(cedula);
			if (medicoSeleccionado == NULL) {
				MessageBox(hwnd, "No se encontró el médico.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			modificar = true; // Cambiar el estado a modificar
			// Llenar campos
			EnableWindow(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), FALSE);
			SetWindowTextA(GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO), medicoSeleccionado->nombre);
			SetWindowTextA(GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO), medicoSeleccionado->apellidoPaterno);
			SetWindowTextA(GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO), medicoSeleccionado->apellidoMaterno);
			SetWindowTextA(GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO), medicoSeleccionado->correo);

			char tel[20];
			sprintf(tel, "%ld", medicoSeleccionado->telefono);
			SetWindowTextA(GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO), tel);

			// Seleccionar especialidad en combo
			HWND hCombo = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
			int count = SendMessage(hCombo, CB_GETCOUNT, 0, 0);
			for (int i = 0; i < count; i++) {
				int id = SendMessage(hCombo, CB_GETITEMDATA, i, 0);
				if (id == medicoSeleccionado->idEspecialidad) {
					SendMessage(hCombo, CB_SETCURSEL, i, 0);
					break;
				}
			}

			// Seleccionar estatus
			CheckRadioButton(hwnd, IDC_ACTIVO_MEDICO, IDC_INACTIVO_MEDICO,
				medicoSeleccionado->estatus ? IDC_ACTIVO_MEDICO : IDC_INACTIVO_MEDICO);
		} break;

		case IDC_LIMPIAR_MEDICO: {
			SetWindowTextA(GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO), "");
			modificar = false; // Reiniciar el estado de modificación
			medicoSeleccionado = NULL; // Reiniciar el médico seleccionado
			// Desmarcar estatus
			CheckRadioButton(hwnd, IDC_ACTIVO_MEDICO, IDC_INACTIVO_MEDICO, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), TRUE);
			// Reiniciar ComboBox
			SendMessage(GetDlgItem(hwnd, COMBO_ESPECIALIDAD), CB_SETCURSEL, -1, 0);
		} break;

		}

	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK vPaciente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Paciente* pacienteActual = NULL; // Para guardar el paciente encontrado y modificarlo
	switch (msg)
	{
		case WM_INITDIALOG: {
			modificar = false;
			HWND hListPacientes = GetDlgItem(hwnd, LIST_INFO_PACIENTES);
			llenarListaPacientes(hListPacientes, primeroPaciente); // donde cabezaPacientes es el inicio de la lista enlazada
		} break;
		case WM_COMMAND: {
			menu(hwnd, wParam);
			switch (LOWORD(wParam)) {
			case APLICAR_PACIENTE_BTN: {
				HWND hNombrePaciente = GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE);
				HWND hPaternoPaciente = GetDlgItem(hwnd, PATERNO_INFO_PACIENTE);
				HWND hMaternoPaciente = GetDlgItem(hwnd, MATERNO_INFO_PACIENTE);
				HWND hIdPaciente = GetDlgItem(hwnd, ID_INFO_PACIENTE);
				HWND hEmail = GetDlgItem(hwnd, EMAIL_INFO_PACIENTE);
				HWND hTelefono = GetDlgItem(hwnd, TEL_INFO_PACIENTE);
				HWND hFechaNac = GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE);
				HWND hSexoM = GetDlgItem(hwnd, HOMBRE_PACIENTE);
				HWND hSexoF = GetDlgItem(hwnd, MUJER_PACIENTE);
				HWND hActivo = GetDlgItem(hwnd, ACTIVO_PACIENTE);
				HWND hInactivo = GetDlgItem(hwnd, INACTIVO_PACIENTE);

				char nombre[50], paterno[50], materno[50], correo[50], buffer[20];
				int idPaciente = 0;
				long telefono = 0;
				SYSTEMTIME fechaNacimiento;

				// Obtener textos
				GetWindowTextA(hNombrePaciente, nombre, 50);
				GetWindowTextA(hPaternoPaciente, paterno, 50);
				GetWindowTextA(hMaternoPaciente, materno, 50);
				GetWindowTextA(hIdPaciente, buffer, 20);

				if (strlen(buffer) == 0 && !modificar) {
					MessageBox(hwnd, "El id del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				if (sscanf(buffer, "%d", &telefono) != 1 && !modificar) {
					MessageBox(hwnd, "Id inválido.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				GetWindowTextA(hEmail, correo, 50);
				GetWindowTextA(hTelefono, buffer, 20);

				if (strlen(buffer) == 0) {
					MessageBox(hwnd, "El teléfono del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				if (sscanf(buffer, "%d", &telefono) != 1) {
					MessageBox(hwnd, "Teléfono inválido.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				telefono = atol(buffer);

				// Validaciones de campos
				if (strlen(nombre) == 0) {
					MessageBox(hwnd, "El nombre del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(paterno) == 0) {
					MessageBox(hwnd, "El apellido paterno del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(materno) == 0) {
					MessageBox(hwnd, "El apellido materno del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(correo) == 0) {
					MessageBox(hwnd, "El correo electrónico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				
				if (!IsDlgButtonChecked(hwnd, HOMBRE_PACIENTE) && !IsDlgButtonChecked(hwnd, MUJER_PACIENTE)) {
					MessageBox(hwnd, "Debe seleccionar un sexo para el paciente.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (!IsDlgButtonChecked(hwnd, ACTIVO_PACIENTE) && !IsDlgButtonChecked(hwnd, INACTIVO_PACIENTE)) {
					MessageBox(hwnd, "Debe seleccionar un estatus para el paciente.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Obtener la fecha del DateTimePicker
				DateTime_GetSystemtime(hFechaNac, &fechaNacimiento);

				// Validar que la fecha no sea mayor a la de ayer
				SYSTEMTIME hoy, ayer;
				GetLocalTime(&hoy);
				ayer = hoy;
				if (ayer.wDay > 1) {
					ayer.wDay--;
				}
				else {
					if (ayer.wMonth == 1) {
						ayer.wYear--;
						ayer.wMonth = 12;
					}
					else {
						ayer.wMonth--;
					}
					// Esto es una simplificación. Para precisión real, deberías usar funciones de tiempo.
					ayer.wDay = 28; // Aproximación aceptable
				}

				if (CompareSystemTime(fechaNacimiento, ayer) > 0) {
					MessageBox(hwnd, "La fecha de nacimiento debe ser anterior al día de ayer.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Sexo y estatus
				SEXO sexo = IsDlgButtonChecked(hwnd, HOMBRE_PACIENTE) ? MASCULINO : FEMENINO;
				bool estatus = IsDlgButtonChecked(hwnd, ACTIVO_PACIENTE);

				// ID de paciente
				GetWindowTextA(hIdPaciente, buffer, 20);
				idPaciente = atoi(buffer);

				if (!modificar) {
					if (buscarPaciente(idPaciente)) {
						MessageBox(hwnd, "Ya existe un paciente con ese ID.", "Error", MB_OK | MB_ICONERROR);
						break;
					}
					Paciente* nuevo = crearPaciente(idPaciente, nombre, paterno, materno, correo, telefono, fechaNacimiento, sexo, estatus);
					agregarPaciente(nuevo);

					MessageBox(hwnd, "Paciente agregado exitosamente.", "Información", MB_OK | MB_ICONINFORMATION);
				}
				else {
					// Modo modificar
					//static Paciente* pacienteMod = NULL;
					if (pacienteActual) {
						strcpy(pacienteActual->nombre, nombre);
						strcpy(pacienteActual->apellidoPaterno, paterno);
						strcpy(pacienteActual->apellidoMaterno, materno);
						strcpy(pacienteActual->correo, correo);
						pacienteActual->telefono = telefono;
						pacienteActual->fechaNacimiento = fechaNacimiento;
						pacienteActual->sexo = sexo;
						pacienteActual->estatus = estatus;

						MessageBox(hwnd, "Paciente modificado correctamente.", "Información", MB_OK | MB_ICONINFORMATION);
					}
					modificar = false;
				}

				// Actualizar lista y limpiar campos (opcional)
				llenarListaPacientes(GetDlgItem(hwnd, LIST_INFO_PACIENTES), primeroPaciente);

			} break;

			case BUSCAR_PACIENTE_BTN: {
				char buffer[20];
				GetWindowTextA(GetDlgItem(hwnd, IDC_BUSCAR_PACIENTE), buffer, 20);
				int id = atoi(buffer);
				pacienteActual = buscarPaciente(id);
				if (pacienteActual == NULL) {
					MessageBox(hwnd, "Paciente no encontrado.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				EnableWindow(GetDlgItem(hwnd, ID_INFO_PACIENTE), FALSE);

				char idPaciente[20];
				sprintf_s(idPaciente, "%d", pacienteActual->idPaciente);

				SetWindowTextA(GetDlgItem(hwnd, ID_INFO_PACIENTE), idPaciente);
				SetWindowTextA(GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE), pacienteActual->nombre);
				SetWindowTextA(GetDlgItem(hwnd, PATERNO_INFO_PACIENTE), pacienteActual->apellidoPaterno);
				SetWindowTextA(GetDlgItem(hwnd, MATERNO_INFO_PACIENTE), pacienteActual->apellidoMaterno);
				SetWindowTextA(GetDlgItem(hwnd, EMAIL_INFO_PACIENTE), pacienteActual->correo);
				sprintf_s(buffer, "%ld", pacienteActual->telefono);
				SetWindowTextA(GetDlgItem(hwnd, TEL_INFO_PACIENTE), buffer);
				DateTime_SetSystemtime(GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE), GDT_VALID, &pacienteActual->fechaNacimiento);
				CheckRadioButton(hwnd, HOMBRE_PACIENTE, MUJER_PACIENTE, pacienteActual->sexo == MASCULINO ? HOMBRE_PACIENTE : MUJER_PACIENTE);
				CheckRadioButton(hwnd, ACTIVO_PACIENTE, INACTIVO_PACIENTE, pacienteActual->estatus ? ACTIVO_PACIENTE : INACTIVO_PACIENTE);
				modificar = true;
			} break;

			case IDC_LIMPIAR_PACIENTE: {
				SetWindowTextA(GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, PATERNO_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, MATERNO_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, ID_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, EMAIL_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, TEL_INFO_PACIENTE), "");
				EnableWindow(GetDlgItem(hwnd, ID_INFO_PACIENTE), TRUE);
				DateTime_SetSystemtime(GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE), GDT_NONE, NULL);
				CheckRadioButton(hwnd, HOMBRE_PACIENTE, MUJER_PACIENTE, 0);
				CheckRadioButton(hwnd, ACTIVO_PACIENTE, INACTIVO_PACIENTE, 0);
				modificar = false;
				pacienteActual = NULL;
			} break;

			case CITA_BTN: {


			} break;

			}
			break;
		case WM_CLOSE: {
			guardarArchivosBinarios();
			PostQuitMessage(0);
		} break;
		}
	}
		return FALSE;
	
}

LRESULT CALLBACK ventanaEspecialidad(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Especialidad* espSeleccionada = NULL; // Puntero persistente

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		llenarListaEspecialidades(hwnd);
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);

		switch (LOWORD(wParam)) {

		case APLICAR_ESP_BTN: {
			HWND hIdEspecialidad = GetDlgItem(hwnd, ID_INFO_ESP);
			HWND hNombreEsp = GetDlgItem(hwnd, NOMBRE_INFO_ESP);

			int idEspecialidad = 0;
			char nombre[50];
			char buffer[20];

			GetWindowTextA(hNombreEsp, nombre, 50);
			GetWindowTextA(hIdEspecialidad, buffer, 20);

			if (strlen(nombre) == 0 || strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "Todos los campos son obligatorios.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &idEspecialidad) != 1 && !modificar) {
				MessageBox(hwnd, "ID inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (espSeleccionada != NULL) {
				// Modificar la especialidad existente
				//espSeleccionada->idEspecialidad = idEspecialidad;
				strcpy(espSeleccionada->nombre, nombre);
				MessageBox(hwnd, "Especialidad modificada correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				// Validar que no exista ese ID
				if (buscarEspecialidad(raiz, idEspecialidad)) {
					MessageBox(hwnd, "Ya existe una especialidad con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				// Crear y agregar nueva especialidad
				Especialidad* nueva = crearEspecialidad(idEspecialidad, nombre);
				agregarEspecialidad(raiz, nueva);
				MessageBox(hwnd, "Especialidad agregada correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			//espSeleccionada = NULL; // Reiniciar después de aplicar
			llenarListaEspecialidades(hwnd);
		} break;

		case BUSCAR_ESP_BTN: {
			char buffer[20];
			HWND hIdEspecialidad = GetDlgItem(hwnd, IDC_BUSCAR_ESP);
			GetWindowTextA(hIdEspecialidad, buffer, 20);
			int idBuscado = 0;

			if (sscanf(buffer, "%d", &idBuscado) != 1) {
				MessageBox(hwnd, "Ingresa un ID válido.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			espSeleccionada = buscarEspecialidad(raiz, idBuscado);
			if (espSeleccionada) {
				SetDlgItemTextA(hwnd, NOMBRE_INFO_ESP, espSeleccionada->nombre);
				modificar = true; // Cambiar el estado a modificar
				EnableWindow(GetDlgItem(hwnd, ID_INFO_ESP), FALSE);
				MessageBox(hwnd, "Especialidad encontrada. Puedes modificarla.", "Encontrado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(hwnd, "Especialidad no encontrada.", "Error", MB_OK | MB_ICONERROR);
			}
		} break;

		case LIMPIAR_ESP_BTN: {
			EnableWindow(GetDlgItem(hwnd, ID_INFO_ESP), TRUE);
			SetDlgItemTextA(hwnd, ID_INFO_ESP, "");
			SetDlgItemTextA(hwnd, NOMBRE_INFO_ESP, "");
			modificar = false; // Reiniciar el estado de modificación
			espSeleccionada = NULL; // Limpiar selección
		} break;
		}
	} break;

	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK vConsultorio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Consultorio* consultorioSeleccionado = NULL; // Puntero persistente

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		llenarComboMedicos(hwnd);
		llenarListaConsultorios(hwnd);
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);

		switch (LOWORD(wParam)) {

			// ===== MANEJAR SELECCIÓN EN COMBO DE MÉDICOS =====
		case COMBO_MEDICOS: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int cedulaMedico = obtenerCedulaDelCombo(hwnd);
				if (cedulaMedico != -1) {
					llenarEspecialidadDelMedico(hwnd, cedulaMedico);
				}
			}
		} break;

						  // ===== BUSCAR CONSULTORIO =====
		case BTN_BUSCAR_CONSULTORIO: {
			
			char buffer[20];
			HWND hBuscar = GetDlgItem(hwnd, BUSCAR_CONSULTORIO);
			GetWindowTextA(hBuscar, buffer, 20);
			int idBuscado = 0;

			if (sscanf(buffer, "%d", &idBuscado) != 1) {
				MessageBox(hwnd, "Ingresa un ID válido.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			consultorioSeleccionado = buscarConsultorio(primeroConsultorio, idBuscado);
			if (consultorioSeleccionado) {
				//llenarComboMedicos(hwnd); // Asegurarse de que el combo de médicos esté lleno antes de buscar
				// Solo medicos que tengan la especialidad del consultorio
				int especialidadId = 0;
				especialidadId = buscarMedico(consultorioSeleccionado->cedulaMedico)->idEspecialidad;
				llenarComboMedicosEspecialidadConsultorio(hwnd, especialidadId); // llenar combo de médicos por especialidad antes de buscar
				// Llenar campos con datos del consultorio encontrado
				SetDlgItemInt(hwnd, IDC_ID_CONSULTORIO, consultorioSeleccionado->idConsultorio, FALSE);

				// Seleccionar el médico en el combo
				HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
				int items = SendMessage(hCombo, CB_GETCOUNT, 0, 0);



				bool doctorInactivo = true;

				for (int i = 0; i < items; i++) {
					char buffer[100];
					SendMessageA(hCombo, CB_GETLBTEXT, i, (LPARAM)buffer);
					int cedula = 0;
					if (sscanf(buffer, "%d", &cedula) == 1 && cedula == consultorioSeleccionado->cedulaMedico) {
						SendMessage(hCombo, CB_SETCURSEL, i, 0);
						// Llenar especialidad automáticamente
						doctorInactivo = false; // Si se encuentra, no es inactivo
						llenarEspecialidadDelMedico(hwnd, cedula);
						break;
					}
				}

				if (doctorInactivo) {
					MessageBox(hwnd, "El médico asignado al consultorio está inactivo.", "Advertencia", MB_OK | MB_ICONWARNING);
					SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");

					// Limpiar selección del combo
					HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
					SendMessage(hCombo, CB_SETCURSEL, -1, 0);
				}
				

				modificar = true;
				EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), FALSE);
				//MessageBox(hwnd, "Consultorio encontrado. Puedes modificarlo.", "Encontrado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(hwnd, "Consultorio no encontrado.", "Error", MB_OK | MB_ICONERROR);
			}
		} break;

								   // ===== LIMPIAR FORMULARIO =====
		case IDC_LIMPAR_CONSULTORIO: {
			EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), TRUE);
			SetDlgItemTextA(hwnd, IDC_ID_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, BUSCAR_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");

			// Limpiar selección del combo
			HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
			SendMessage(hCombo, CB_SETCURSEL, -1, 0);

			modificar = false;
			consultorioSeleccionado = NULL;
			MessageBox(hwnd, "Formulario limpiado.", "Limpiar", MB_OK | MB_ICONINFORMATION);
		} break;

								   // ===== APLICAR (CREAR O MODIFICAR) =====
		case IDC_APLICAR_CONSULTORIO: {
			HWND hIdConsultorio = GetDlgItem(hwnd, IDC_ID_CONSULTORIO);

			int idConsultorio = 0;
			char buffer[20];
			GetWindowTextA(hIdConsultorio, buffer, 20);

			// Validaciones básicas
			if (strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "El ID del consultorio es obligatorio.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &idConsultorio) != 1 && !modificar) {
				MessageBox(hwnd, "ID inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Obtener médico seleccionado
			int cedulaMedico = obtenerCedulaDelCombo(hwnd);
			if (cedulaMedico == -1) {
				MessageBox(hwnd, "Debe seleccionar un médico.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que el médico no tenga ya un consultorio asignado (relación 1:1)
			/*if (!modificar) {
				
			}*/
			Consultorio* actual = primeroConsultorio;
			while (actual != NULL) {
				if (actual->cedulaMedico == cedulaMedico) {
					MessageBox(hwnd, "Este médico ya tiene un consultorio asignado.", "Error", MB_OK | MB_ICONERROR);
					return FALSE;
				}
				actual = actual->siguiente;
			}

			if (consultorioSeleccionado != NULL) {
				// MODIFICAR consultorio existente
				consultorioSeleccionado->cedulaMedico = cedulaMedico;
				consultorioSeleccionado->estatus = DISPONIBLE; // Estado por defecto

				MessageBox(hwnd, "Consultorio modificado correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
				llenarComboMedicos(hwnd); // Asegurarse de que el combo de médicos esté lleno después de modificar
			}
			else {
				// CREAR nuevo consultorio

				// Validar que no exista ese ID
				if (buscarConsultorio(primeroConsultorio, idConsultorio)) {
					MessageBox(hwnd, "Ya existe un consultorio con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Crear y agregar nuevo consultorio
				Consultorio* nuevo = crearConsultorio(idConsultorio, DISPONIBLE);
				nuevo->cedulaMedico = cedulaMedico;
				agregarConsultorio(nuevo);

				MessageBox(hwnd, "Consultorio agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			// Limpiar formulario después de aplicar
			EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), TRUE);
			SetDlgItemTextA(hwnd, IDC_ID_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, BUSCAR_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");
			SendMessage(GetDlgItem(hwnd, COMBO_MEDICOS), CB_SETCURSEL, -1, 0);

			modificar = false;
			consultorioSeleccionado = NULL;
			llenarListaConsultorios(hwnd);
		} break;
		}
	} break;

	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK ventanaReporteCitasMedico(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
		/*case REGRESAR_BTN: {
			EndDialog(hwnd, 0);
			DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
		} break;*/
		}
	} break;
	case WM_CLOSE: {
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK ventanaReporteCitasPaciente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
			/*case REGRESAR_BTN: {
				EndDialog(hwnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
			} break;*/
		}
	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}

void menu(HWND hwnd, WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case ID_MEDICO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MEDICO), NULL, vDoctor);
	} break;
	case ID_PACIENTE:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
	} break;
	case ID_ESPECIALIDAD:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_ESPECIALIDAD), NULL, ventanaEspecialidad);
	} break;
	case ID_CONSULTORIO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CONSULTORIO), NULL, vConsultorio);
	} break;
	case ID_REPORTEDECITASMEDICO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_REPORTE_CITAS_MEDICO), NULL, ventanaReporteCitasMedico);
	} break;
	case ID_REPORTEDECITASPACIENTE:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_REPORTE_PACIENTE), NULL, ventanaReporteCitasPaciente);
	} break;
	}
}

int CompareSystemTime(SYSTEMTIME t1, SYSTEMTIME t2) {
	FILETIME f1, f2;
	SystemTimeToFileTime(&t1, &f1);
	SystemTimeToFileTime(&t2, &f2);
	ULONGLONG u1 = (((ULONGLONG)f1.dwHighDateTime) << 32) + f1.dwLowDateTime;
	ULONGLONG u2 = (((ULONGLONG)f2.dwHighDateTime) << 32) + f2.dwLowDateTime;
	return (u1 < u2) ? -1 : (u1 > u2) ? 1 : 0;
}


char carpeta[MAX_PATH];
void InicializarArchivos() {
	char citas_[] = "\\Citas.dat";
	char pacientes_[] = "\\Pacientes.dat";
	char medicos_[] = "\\Medicos.dat";
	char especialidades_[] = "\\Especialidades.dat";
	char consultorio_[] = "\\Consultorios.dat";


	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, citas_);
	strcpy(archivoCitas, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, pacientes_);
	strcpy(archivoPacientes, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, medicos_);
	strcpy(archivoMedicos, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, especialidades_);
	strcpy(archivoEspecialidades, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, consultorio_);
	strcpy(archivoConsultorios, carpeta);
}

void leerArchivosBinarios()
{
	std::ifstream lecturaCitas(archivoCitas, std::ios::binary);
	std::ifstream lecturaPacientes(archivoPacientes, std::ios::binary);
	std::ifstream lecturaMedicos(archivoMedicos, std::ios::binary);
	std::ifstream lecturaEspecialidades(archivoEspecialidades, std::ios::binary);
	std::ifstream lecturaConsultorio(archivoConsultorios, std::ios::binary);

	if (lecturaCitas.is_open())
	{
		leerCitas(lecturaCitas);
		lecturaCitas.close();
	}
	else
		MessageBox(NULL, "Error de lectura en citas", "error", MB_ICONERROR | MB_OK);

	if (lecturaPacientes.is_open())
	{
		leerPacientes(lecturaPacientes);
		lecturaPacientes.close();
	}
	else
		MessageBox(NULL, "Error de lectura en pacientes", "error", MB_ICONERROR | MB_OK);

	if (lecturaMedicos.is_open())
	{
		leerMedicos(lecturaMedicos);
		lecturaMedicos.close();
	}
	else
		MessageBox(NULL, "Error de lectura en medicos", "error", MB_ICONERROR | MB_OK);

	if (lecturaEspecialidades.is_open())
	{
		leerEspecialidad(lecturaEspecialidades);
		lecturaEspecialidades.close();
	}
	else
		MessageBox(NULL, "Error de lectura en especialidades", "error", MB_ICONERROR | MB_OK);

	if (lecturaConsultorio.is_open())
	{
		leerConsultorios(lecturaConsultorio);
		lecturaConsultorio.close();
	}
	else
		MessageBox(NULL, "Error de lectura en consultorios", "error", MB_ICONERROR | MB_OK);
}

void guardarArchivosBinarios()
{
	std::ofstream escrituraCitas(archivoCitas, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraPacientes(archivoPacientes, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraMedicos(archivoMedicos, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraEspecialidades(archivoEspecialidades, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraConsultorio(archivoConsultorios, std::ios::binary | std::ios::trunc);

	if (escrituraCitas.is_open())
	{
		escribirCitas(escrituraCitas);
		escrituraCitas.close();
	}
	else
		MessageBox(NULL, "Error de escritura en citas", "error", MB_ICONERROR | MB_OK);

	if (escrituraPacientes.is_open())
	{
		escribirPacientes(escrituraPacientes);
		escrituraPacientes.close();
	}
	else
		MessageBox(NULL, "Error de escritura en pacientes", "error", MB_ICONERROR | MB_OK);

	if (escrituraMedicos.is_open())
	{
		escribirMedicos(escrituraMedicos);
		escrituraMedicos.close();
	}
	else
		MessageBox(NULL, "Error de escritura en medicos", "error", MB_ICONERROR | MB_OK);

	if (escrituraEspecialidades.is_open())
	{
		escribirEspecialidad(raiz, escrituraEspecialidades);
		escrituraEspecialidades.close();
	}
	else
		MessageBox(NULL, "Error de escritura en especialidades", "error", MB_ICONERROR | MB_OK);

	if (escrituraConsultorio.is_open())
	{
		escribirConsultorios(escrituraConsultorio);
		escrituraConsultorio.close();
	}
	else
		MessageBox(NULL, "Error de escritura en consultorios", "error", MB_ICONERROR | MB_OK);
}
