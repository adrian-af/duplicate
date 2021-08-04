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
public:
    string nombre;
    int tam;
    string nombreRuta;
    string tipo;
    string hash;
    bool duplicado;
    time_t modificado;
};

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "spanish");

    //solicitar la ruta al usuario
    string ruta;
    int elect, criterio;
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
    cout << "¿Qué deseas hacer con los archivos duplicados que se detecten?" << endl;
    cout << "1. Eliminar uno de los dos" << endl;
    cout << "2. Decidir individualmente" << endl;
    int in = 0;
    while(in == 0)
    {
        cin >> elect;
        if(elect == 1)
        {
            cout << "¿Qué criterio quieres que se siga para la eliminación? Borrar el archivo..." << endl;
            cout << "1. Más reciente" << endl;
            cout << "2. Más antiguo" << endl;
            cout << "3. Con el nombre más largo" << endl;
            cout << "4. Con el nombre más corto" << endl;
            cout << "5. Con la ruta más larga" << endl;
            cout << "6. Con la ruta más corta" << endl;
            cin >> criterio;
            while(criterio < 0 || criterio > 6)
            {
                cout << "Introduce un número entre 1 y 6 para elegir el criterio de eliminación." << endl;
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

                vectorArchivos[contador].nombre = elemento->d_name;
                vectorArchivos[contador].nombreRuta = ruta + vectorArchivos[contador].nombre;
                stat((vectorArchivos[contador].nombreRuta).c_str(), &buff);

                switch (buff.st_mode & S_IFMT) {
                    case S_IFDIR:  //si es un directorio, hay que abrirlo y volver a hacer todo esto
                    {
                        break;
                    }

                    case S_IFREG:  //si es un archivo, se obtiene el tamaño para comparar
                    {
                        if(vectorArchivos[contador].nombre != "." && vectorArchivos[contador].nombre != "..")
                        {
                            vectorArchivos[contador].tam = buff.st_size;
                            vectorArchivos[contador].modificado = buff.st_mtime;
                            cout << vectorArchivos[contador].nombre << " es un archivo de " << vectorArchivos[contador].tam << " bytes" << endl;
                            vectorArchivos[contador].tipo = "file";
                            break;
                        }

                    }
                    default:     //si no se reconoce el tipo, se le muestra al usuario
                    {
                        if(vectorArchivos[contador].nombre != "." && vectorArchivos[contador].nombre != "..")
                        {
                            //cout << "archivo " << vectorArchivos[contador].nombre << " es de un tipo desconocido." << endl;
                            vectorArchivos[contador].tipo = "file";
                            vectorArchivos[contador].tam = buff.st_size;
                            vectorArchivos[contador].modificado = buff.st_mtime;
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
        cout << "no entra en el if";
        exit(-1);
    }
    closedir(directorio);
    int tamVector;
    tamVector=vectorArchivos.size();
    cout << "tamVector es " << tamVector << endl;
    //cout << tamVector-3 << " tiene " << vectorArchivos[tamVector-3].tam << " bytes" << endl;

    for(i=0; i<tamVector; i++)
    {
        for(j=0; j<tamVector; j++)
        {
            if((vectorArchivos[i].tam == vectorArchivos[j].tam) && (i!=j) && (vectorArchivos[i].tipo == "file") && (vectorArchivos[j].tipo == "file"))
            {
                //vectorArchivos[i] y vectorArchivos[j] son del mismo tamaño
                MD5 md5;

                int n1 = (vectorArchivos[i].nombreRuta).length();
                char nom1[256];
                strcpy(nom1, (vectorArchivos[i].nombreRuta).c_str());
                string hash1 (md5.digestFile(nom1));
                vectorArchivos[i].hash = hash1;

                int n2 = (vectorArchivos[j].nombreRuta).length();
                char nom2[256];
                strcpy(nom2, (vectorArchivos[j].nombreRuta).c_str());
                string hash2 (md5.digestFile(nom2));
                vectorArchivos[j].hash = hash2;
                if(vectorArchivos[i].hash == vectorArchivos[j].hash)
                {
                    //vectorArchivos[i] y vectorArchivos[j] tienen el mismo hash

                    FILE *pfile1;
                    FILE *pfile2;
                    char contenido1[256], contenido2[256];

                    vectorArchivos[i].duplicado = true;
                    vectorArchivos[j].duplicado = true;
                    pfile1 = fopen(nom1, "rb");
                    pfile2 = fopen(nom2, "rb");
                    while(fgets(contenido1, 256, pfile1) && fgets(contenido2, 256, pfile2))
                    {
                        if(contenido1 != contenido2)
                        {
                            break; //los archivos no son iguales
                            vectorArchivos[i].duplicado = false;
                            vectorArchivos[j].duplicado = false;
                        }
                        else
                        {
                            //contenido1[0] = "\0"; vaciar las variables para seguir el bucle con la siguiente línea

                        }
                    }
                    fclose(pfile1);
                    fclose(pfile2);

                    if(vectorArchivos[i].duplicado == true && vectorArchivos[j].duplicado == true && vectorArchivos[i].hash == vectorArchivos[j].hash)
                    {
                        cout << vectorArchivos[i].nombre << " y " << vectorArchivos[j].nombre << " son duplicados" << endl;
                        if(elect == 2) //Decidir manualmente
                        {
                            int eliminar;
                            cout << "Pulsa 1 para borrar el archivo " << vectorArchivos[i].nombre << ", 2 para borrar " << vectorArchivos[j].nombre << " o cualquier otra tecla para no borrar ninguno." << endl;
                            cin>>eliminar;
                            if(eliminar == 1)
                            {
                                //eliminar vectorArchivo[i].nombreRuta
                                int n = vectorArchivos[i].nombreRuta.length();
                                char rutaBorrar[n+1];
                                strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                if(remove(rutaBorrar) != 0 )
                                {
                                    perror("Error al borrar el archivo");
                                }
                                else
                                {
                                    cout << "El archivo se borró con exito!" << endl;
                                    vectorArchivos[j].duplicado = false;
                                }
                            }
                            else if(eliminar == 2)
                            {
                                //eliminar vectorArchivo[j].nombreRuta
                                int n = vectorArchivos[j].nombreRuta.length();
                                char rutaBorrar[n+1];
                                strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                if(remove(rutaBorrar) != 0 )
                                {
                                    perror("Error al borrar el archivo");
                                }
                                else
                                {
                                    cout << "El archivo se borró con exito!" << endl;
                                    vectorArchivos[i].duplicado = false;
                                }
                            }
                        }
                        else if(elect == 1) //Borrar automáticamente
                        {
                            if(criterio == 1) //Borrar el archivo más reciente
                            {
                                if(vectorArchivos[i].modificado > vectorArchivos[j].modificado)
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
                                    }
                                }
                            }
                            else if(criterio == 2) //Borrar el archivo menos reciente
                            {
                                if(vectorArchivos[i].modificado < vectorArchivos[j].modificado)
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
                                    }
                                }
                            }
                            else if(criterio == 3) //Borrar el archivo con el nombre más largo
                            {
                                if((vectorArchivos[i].nombre).length() > (vectorArchivos[j].nombre).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
                                    }
                                }
                            }
                            else if(criterio == 4) //Borrar el archivo con el nombre más corto
                            {
                                if((vectorArchivos[i].nombre).length() < (vectorArchivos[j].nombre).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
                                    }
                                }
                            }
                            else if(criterio == 5) //Borrar el archivo con la ruta más larga
                            {
                                if((vectorArchivos[i].nombreRuta).length() > (vectorArchivos[j].nombreRuta).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
                                    }
                                }
                            }
                            else if(criterio == 6) //Borrar el archivo con la ruta más corta
                            {
                                if((vectorArchivos[i].nombreRuta).length() < (vectorArchivos[j].nombreRuta).length())
                                {
                                    //Borrar vectorArchivos[i].nombreRuta
                                    int n = vectorArchivos[i].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[i].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[j].duplicado = false;
                                    }
                                }
                                else
                                {
                                    //Borrar vectorArchivos[j].nombreRuta
                                    int n = vectorArchivos[j].nombreRuta.length();
                                    char rutaBorrar[n+1];
                                    strcpy(rutaBorrar, (vectorArchivos[j].nombreRuta).c_str());
                                    if(remove(rutaBorrar) != 0 )
                                    {
                                        perror("Error al borrar el archivo.");
                                    }
                                    else
                                    {
                                        cout << "El archivo se borró con exito!" << endl;
                                        vectorArchivos[i].duplicado = false;
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

    cout << "hay " << tamVector << " archivos" << endl;
    for(i=0; i<tamVector; i++)
    {

        cout << "archivo " << i << ": " << vectorArchivos[i].nombreRuta << " tiene " << vectorArchivos[i].tam << " bytes." << endl;
    }

    string eof;
    cout << "Fin del programa. Pulsa cualquier tecla e intro para salir." << endl;
    cin >> eof;
    return 0;
}
