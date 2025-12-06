#include <fstream>
#include <random>
#include "include/mrl_sketch.hpp"

int rankOf(vector<int>& sorted, int value){
    int rank = 0;
    for (int num: sorted){
        if (num > value){break;}
        rank++;
    }
    return rank;
}

int quantile(vector<int>& sorted, float value){
    return sorted[sorted.size()*value];
}

int main(int argc, char *argv[])
{
    // Lee argumentos FILENAME, EPSILON y TIMES
    string filename = argv[1]; 
    float epsilon = stof(argv[2]); int value;
    int times = stoi(argv[3]);
    ifstream stream(filename);

    // Abre el archivo especificado y almacena su valores
    // Extrae el tamaño del vector (n)
    cout << "File to insert: " << filename << endl;
    vector<int> values;
    while (stream >> value){values.push_back(value);}
    vector<int> sorted_values = values;
    sort(sorted_values.begin(), sorted_values.end());
    size_t n = values.size();

    // Construye el sketch con el vector de datos y
    // los parametros especificados
    cout << "Constructing MRL sketch..." << endl;
    mrl::MRLSketch sketch(epsilon, n);
    for (int num: values){sketch.insert(num);}    

    cout << "MRL sketch constructed successfully." << endl;
    cout << "Size: " << n << ", epsilon: " << epsilon << endl;
    cout << "Obtaining mean rank error. " << times << " iterations." << endl;

    // Crea archivo de salida para almacenar errores de rank 
    // (no esta del todo utilizable, puede escribir datos redundantes)
    ofstream rank_errors; rank_errors.open("rankerrs.csv", ios::app);

    // Generador para valores aleatorios desde 0 hasta el valor máximo del arreglo
    random_device rd; mt19937_64 rng(rd()); uniform_int_distribution<int64_t> unif(0, n-1);

    int test, mrlrank, rrank;
    float r_error, total = 0; 

    // Genera un valor aleatorio la cantidad de veces especificada por TIMES
    // Consulta por el rank del valor y promedia
    for (int i = 0; i < times; i++)
    {
        test = values[unif(rng)];
        mrlrank = sketch.rank(values[test]); rrank = rankOf(sorted_values, test);
        r_error = 1.0*abs(mrlrank - rrank)/n;
        total += r_error;
    }
    total /= times;

    // Escribe el valor en un archivo .csv
    rank_errors << filename.substr(0,filename.length()-4) << "," << epsilon << "," << total << endl;
    cout << "Mean rank error: " << total << endl;

    // Realiza un trabajo similar al anterior, sin embargo solo consulta por los deciles del arreglo
    cout << "Obtaining mean quantiles error..." << endl;
    string q_errors = to_string(epsilon).substr(0,4)+"_"+filename.substr(0,filename.length() - 4)+".csv";
    ofstream error_data; error_data.open(q_errors);
    float q_error;
    error_data << "quantile,error" << endl;
    for (float q = 0.1; q <= 1.0; q += 0.1){
        q_error = abs(1.0*rankOf(sorted_values, sketch.quantile(q))/sorted_values.size() - q);
        cout << "Quantile error (" << q << "): " << q_error << endl;
        error_data << q << "," << q_error << endl;
    }
    cout << "Quantile errors stored at: " << q_errors << endl;
    return 0;
}



