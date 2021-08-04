#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <vector>
#include <time.h>
#include "md5.h"

using namespace std;
int i, j;
class cadaArchivo
{
	//Atributos
private:
    string nombre;
    int tam;
    string nombreRuta;
    string tipo;
    string hash;
    bool duplicado;
    time_t modificado;

    //Métodos
public:
    cadaArchivo()
    {
    	tam = 0;
    	duplicado = false;
    }

    string getNombre()
    {
    	return nombre;
    }
    void changeNombre(string nuevoNombre)
    {
    	nombre = nuevoNombre;
    }

	int getTam()
    {
    	return tam;
    }
    void changeTam(int nuevoTam)
    {
    	tam = nuevoTam;
    }

	string getNombreRuta()
    {
    	return nombreRuta;
    }
    void changeNombreRuta(string nuevoNombreRuta)
    {
    	nombreRuta = nuevoNombreRuta;
    }

    string getTipo()
    {
    	return tipo;
    }
    void changeTipo(string nuevoTipo)
    {
    	tipo = nuevoTipo;
    }

    string getHash()
    {
    	return hash;
    }
    void changeHash(string nuevoHash)
    {
    	hash = nuevoHash;
    }

    bool getDuplicado()
    {
    	return duplicado;
    }
    void changeDuplicado(bool nuevoDuplicado)
    {
    	duplicado = nuevoDuplicado;
    }

    time_t getModificado()
    {
    	return modificado;
    }
    void changeModificado(time_t nuevoModificado)
    {
    	modificado = nuevoModificado;
    }

};

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    //solicitar la ruta al usuario
    string ruta = "";
    int elect, criterio = 0;
    cout << "-----------Adrian Fernandez Ortega - PNL IFCT0609-----------" << endl;
    cout << "Este programa detecta archivos duplicados y los gestiona." << endl;
    cout << "Primero, elige la ruta en la que buscar archivos duplicados" << endl;
    getline(cin,ruta);
    cout << "La ruta que has elegido es: " << ruta << endl;
    string lastRuta = ruta.substr(ruta.length()-1, ruta.length());
    if(lastRuta != "\\")
    {
        ruta = ruta + char(92); //si la ruta no tiene una \ al final, se añade
    }

    //solicitar al usuario que elija cómo se gestionan los duplicados
    cout << "Que deseas hacer con los archivos duplicados que se detecten?" << endl;
    cout << "1. Eliminar uno de los dos" << endl;
    cout << "2. Decidir individualmente" << endl;
    int in = 0;
    while(in == 0)
    {
        cin >> elect;
        if(elect == 1)
        {
            cout << "Que criterio quieres que se siga para la eliminacion? Borrar el archivo..." << endl;
            cout << "1. Mas reciente" << endl;
            cout << "2. Mas antiguo" << endl;
            cout << "3. Con el nombre mas largo" << endl;
            cout << "4. Con el nombre mas corto" << endl;
            cout << "5. Con la ruta mas larga" << endl;
            cout << "6. Con la ruta mas corta" << endl;
            cin >> criterio;
            while(criterio < 0 || criterio > 6)
            {
                cout << "Introduce un numero entre 1 y 6 para elegir el criterio de eliminacion." << endl;
                cin >> criterio;
            }
            in = 1;
        }
        else if(elect == 2)
        {
            cout << "Ok" << endl;

            in = 1;
        }
        else
        {
            cout << "Introduce 1 para eliminar uno de los dos o 2 para decidir individualmente" << endl;
        }
    }
    //leer el directorio
    lectura:
    DIR * directorio;
    struct dirent * elemento;
    struct stat buff;
    string elem, type;
    int contador=0;
    vector <cadaArchivo> vectorArchivos;
    if(directorio = opendir(ruta.c_str()))
    {
        while (elemento = readdir(directorio))
        {
            string nam = elemento->d_name;
            if(nam != "." && nam != ".." )
            {
                cadaArchivo nuevoArchivo;
                vectorArchivos.push_back(nuevoArchivo);

                vectorArchivos[contador].changeNombre(elemento->d_name);
            	vectorArchivos[contador].changeNombreRuta(ruta + vectorArchivos[contador].getNombre());

                stat((vectorArchivos[contador].getNombreRuta()).c_str(), &buff);

                switch (buff.st_mode & S_IFMT) {
                    case S_IFDIR:  //si es un directorio, hay que abrirlo y volver a hacer todo esto
                    {

                        break;
                    }

                    case S_IFREG:  //si es un archivo, se obtiene el tamaño para comparar
                    {
                        if(vectorArchivos[contador].getNombre() != "." && vectorArchivos[contador].getNombre() != "..")
                        {
                            vectorArchivos[contador].changeTam(buff.st_size);
                            vectorArchivos[contador].changeModificado(buff.st_mtime);
                            cout << vectorArchivos[contador].getNombre() << " es un archivo de " << vectorArchivos[contador].getTam() << " bytes" << endl;
                            vectorArchivos[contador].changeTipo("file");
                            break;
                        }

                    }
                    default:     //si no se reconoce el tipo, se le muestra al usuario
                    {
                        if(vectorArchivos[contador].getNombre() != "." && vectorArchivos[contador].getNombre() != "..")
                        {
                            cout << "archivo " << vectorArchivos[contador].getNombre() << " es de un tipo desconocido." << endl;
                            vectorArchivos[contador].changeTipo("file");
                            vectorArchivos[contador].changeTam(buff.st_size);
                            vectorArchivos[contador].changeModificado(buff.st_mtime);
                            break;
                        }
                    }

                }
                contador++;
            }
        }
    }
    else
    {
        cout << "Error: Comprueba que la ruta incluida no tenga caracteres especiales y vuelve a intentarlo.";
        exit(-1);
    }
    closedir(directorio);
    int tamVector;
    tamVector=vectorArchivos.size();
    //cout << "tamVector es " << tamVector << endl;
    //cout << tamVector-3 << " tiene " << vectorArchivos[tamVector-3].tam << " bytes" << endl;

    for(i=0; i<tamVector; i++)
    {
        for(j=0; j<tamVector; j++)
        {
            if((vectorArchivos[i].getTam() == vectorArchivos[j].getTam()) && (i!=j) && (vectorArchivos[i].getTipo() == "file") && (vectorArchivos[j].getTipo() == "file"))
            {
                //vectorArchivos[i] y vectorArchivos[j] son del mismo tamaño
                MD5 md5;

                int n1 = (vectorArchivos[i].getNombreRuta()).length();
                char nom1[256];
                strcpy(nom1, (vectorArchivos[i].getNombreRuta()).c_str());
                string hash1 (md5.digestFile(nom1));
                vectorArchivos[i].changeHash(hash1);

                int n2 = (vectorArchivos[j].getNombreRuta()).length();
                char nom2[256];
                strcpy(nom2, (vectorArchivos[j].getNombreRuta()).c_str());
                string hash2 (md5.digestFile(nom2));
                vectorArchivos[j].changeHash(hash2);
                if(vectorArchivos[i].getHash() == vectorArchivos[j].getHash())
                {
                    //vectorArchivos[i] y vectorArchivos[j] tienen el mismo hash

                    FILE *pfile1;
                    FILE *pfile2;
                    char contenido1[256], contenido2[256];

                    vectorArchivos[i].changeDuplicado(true);
                    vectorArchivos[j].changeDuplicado(true);
                    pfile1 = fopen(nom1, "rb");
                    pfile2 = fopen(nom2, "rb");
                    while(fgets(contenido1, 256, pfile1) && fgets(contenido2, 256, pfile2))
                    {
                        if(contenido1 != contenido2)
                        {
                            break; //los archivos no son iguales
                            vectorArchivos[i].changeDuplicado(false);
                            vectorArchivos[j].changeDuplicado(false);
                        }
                        else
                        {
                            //contenido1[0] = "\0"; vaciar las variables para seguir el bucle con la siguiente línea

                        }
                    }
                    fclose(pfile1);
                    fclose(pfile2);

                    if(vectorArchivos[i].getDuplicado() == true && vectorArchivos[j].getDuplicado() == true && vectorArchivos[i].getHash() == vectorArchivos[j].getHash())
                    {
                        cout << vectorArchivos[i].getNombre() << " y " << vectorArchivos[j].getNombre() << " son duplicados" << endl;
                        if(elect == 2) //Decidir manualmente
                        {
                            int eliminar;
                            cout << "Pulsa 1 para borrar el archivo " << vectorArchivos[i].getNombre() << ", 2 para borrar " << vectorArchivos[j].getNombre() << " o cualquier otro numero para no borrar ninguno." << endl;
                            cin>>eliminar;
                            if(eliminar == 1)
                            {
                                //eliminar vectorArchivo[i].nombreRuta
                                int n = vectorArchivos[i].getNombreRuta().length();
                                char rutaBorrar[n+1];
                                strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                if(remove(rutaBorrar) != 0 )
                                {
                                    perror("Error al borrar el archivo");
                                }
                                else
                                {
                                    cout << "El archivo se borro con exito!" << endl;
                                    vectorArchivos[j].changeDuplicado(false);
                                }
                            }
                            else if(eliminar == 2)
                            {
                                //eliminar vectorArchivo[j].nombreRuta
                                int n = vectorArchivos[j].getNombreRuta().length();
                                char rutaBorrar[n+1];
                                strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                if(remove(rutaBorrar) != 0 )
                                {
                                    perror("Error al borrar el archivo");
                                }
                                else
                                {
                                    cout << "El archivo se borro con exito!" << endl;
                                    vectorArchivos[i].changeDuplicado(false);
                                }
                            }
                        }
                        else if(elect == 1) //Borrar automáticamente
                        {
                            if(criterio == 1) //Borrar el archivo más reciente
                            {
                                if(vectorArchivos[i].getModificado() > vectorArchivos[j].getModificado())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }
                            }
                            else if(criterio == 2) //Borrar el archivo menos reciente
                            {
                                if(vectorArchivos[i].getModificado() < vectorArchivos[j].getModificado())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }
                            }
                            else if(criterio == 3) //Borrar el archivo con el nombre más largo
                            {
                                if((vectorArchivos[i].getNombre()).length() > (vectorArchivos[j].getNombre()).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }
                            }
                            else if(criterio == 4) //Borrar el archivo con el nombre más corto
                            {
                                if((vectorArchivos[i].getNombre()).length() < (vectorArchivos[j].getNombre()).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }
                            }
                            else if(criterio == 5) //Borrar el archivo con la ruta más larga
                            {
                                if((vectorArchivos[i].getNombreRuta()).length() > (vectorArchivos[j].getNombreRuta()).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }
                            }
                            else if(criterio == 6) //Borrar el archivo con la ruta más corta
                            {
                                if((vectorArchivos[i].getNombreRuta()).length() < (vectorArchivos[j].getNombreRuta()).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[j].changeDuplicado(false);
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].getNombreRuta().length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].getNombreRuta()).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borro con exito!" << endl;
                                        vectorArchivos[i].changeDuplicado(false);
                                    }
                                }

                            }
                            else cout << "error con la variable criterio" << endl;
                        } else cout << "error con la variable elect" << endl;
                    }
                }
            }
        }

    }

    string eof;
    cout << "Fin del programa. Pulsa cualquier letra e intro para salir." << endl;
    cin >> eof;
    return 0;
}
