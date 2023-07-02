#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iomanip>

#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;

void Test() {
    using namespace t_catalogue;
    stringstream input("3\nBus bus: a - b\nStop a: 5., 3.\nStop b: 3., 5.\n");
    TransportCatalogue a(input);
    string compare;
    string str;

    //1 case
    input.clear();
    input.str("3\nBus bus\nStop a\nStop b\n");
    double geo_length = ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. }) * 2;
    ostringstream oss;
    int real_length = geo_length;
    oss << setprecision(6) << real_length / geo_length;
    compare = "Bus bus: 3 stops on route, 2 unique stops, " + to_string(real_length) + " route length, " + oss.str() + " curvature\nStop a: buses bus\nStop b: buses bus\n";
    str = a.GetRoutInfo(input);
    cout << str;
    cout << compare;
    for (size_t i = 0; i < str.size();++i) {
        cout << i << " from " << str.size() << "| compare " << compare[i] << " - " << str[i] << " real" << endl;
        assert(compare[i] == str[i]);
    }

    //2 case
    input.clear();
    input.str("4\nBus bus: a > b > c > a\nStop a: 5., 3.\nStop b: 3., 5.\nStop c: 6., 5.\n");
    TransportCatalogue b(input);
    input.clear();
    input.str("4\nBus bus\nStop a\nStop b\nStop c");
    geo_length = ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. }) + ComputeDistance(Coordinates{ 3.,5. }, Coordinates{ 6.,5. }) + ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. });
    real_length = static_cast<int>(ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. })) + static_cast<int>(ComputeDistance(Coordinates{ 3.,5. }, Coordinates{ 6.,5. })) + static_cast<int>(ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. }));
    oss.clear();
    oss.str("");
    oss << setprecision(6) << real_length / geo_length;
    compare = "Bus bus: 4 stops on route, 3 unique stops, " + to_string(real_length) + " route length, " + oss.str() + " curvature\nStop a: buses bus\nStop b: buses bus\nStop c: buses bus\n";
    str = b.GetRoutInfo(input);
    cout << str;
    cout << compare;
    for (size_t i = 0; i < str.size();++i) {
        cout << i << " from " << str.size() << "| compare " << compare[i] << " - " << str[i] << " real" << endl;
        assert(compare[i] == str[i]);
    }

    //3 case
    input.clear();
    input.str("4\nBus bus: a > b > c > a\nStop a: 5., 3., 1000m to b\nStop b: 3., 5.,1000m to c\nStop c: 6., 5., 1000m to a\n");
    TransportCatalogue c(input);
    input.clear();
    input.str("4\nBus bus\nStop a\nStop b\nStop c");
    geo_length = ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. }) + ComputeDistance(Coordinates{ 3.,5. }, Coordinates{ 6.,5. }) + ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. });
    real_length = 3000;
    oss.clear();
    oss.str("");
    oss << setprecision(6) << real_length / geo_length;
    compare = "Bus bus: 4 stops on route, 3 unique stops, " + std::to_string(real_length) + " route length, " + oss.str() + " curvature\nStop a: buses bus\nStop b: buses bus\nStop c: buses bus\n";
    str = c.GetRoutInfo(input);
    cout << str;
    cout << compare;
    for (size_t i = 0; i < str.size();++i) {
        cout << i << " from " << str.size() << "| compare " << compare[i] << " - " << str[i] << " real" << endl;
        assert(compare[i] == str[i]);
    }

    //4 case
    input.clear();
    input.str("4\nBus bus: a > b > c > a\nStop a: 5., 3., 1000m to b, 1000m to c\nStop b: 3., 5.,1000m to c\nStop c: 6., 5.\n");
    TransportCatalogue d(input);
    input.clear();
    input.str("4\nBus bus\nStop a\nStop b\nStop c");
    geo_length = ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. }) + ComputeDistance(Coordinates{ 3.,5. }, Coordinates{ 6.,5. }) + ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. });
    real_length = 3000;
    oss.clear();
    oss.str("");
    oss << setprecision(6) << real_length / geo_length;
    compare = "Bus bus: 4 stops on route, 3 unique stops, " + std::to_string(real_length) + " route length, " + oss.str() + " curvature\nStop a: buses bus\nStop b: buses bus\nStop c: buses bus\n";
    str = d.GetRoutInfo(input);
    cout << str;
    cout << compare;
    for (size_t i = 0; i < str.size();++i) {
        cout << i << " from " << str.size() << "| compare " << compare[i] << " - " << str[i] << " real" << endl;
        assert(compare[i] == str[i]);
    }

    //5 case
    input.clear();
    input.str("4\nBus bus: a > b > c > a\nStop a: 5., 3., 1000m to b\nStop b: 3., 5.,1000m to c\nStop c: 6., 5.\n");
    TransportCatalogue e(input);
    input.clear();
    input.str("4\nBus bus\nStop a\nStop b\nStop c");
    geo_length = ComputeDistance(Coordinates{ 5.,3. }, Coordinates{ 3.,5. }) + ComputeDistance(Coordinates{ 3.,5. }, Coordinates{ 6.,5. }) + ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. });
    real_length = 2000 + ComputeDistance(Coordinates{ 6.,5. }, Coordinates{ 5.,3. });
    oss.clear();
    oss.str("");
    oss << setprecision(6) << real_length / geo_length;
    compare = "Bus bus: 4 stops on route, 3 unique stops, " + std::to_string(real_length) + " route length, " + oss.str() + " curvature\nStop a: buses bus\nStop b: buses bus\nStop c: buses bus\n";
    str = e.GetRoutInfo(input);
    cout << str;
    cout << compare;
    for (size_t i = 0; i < str.size();++i) {
        cout << i << " from " << str.size() << "| compare " << compare[i] << " - " << str[i] << " real" << endl;
        assert(compare[i] == str[i]);
    }


    cout << "Success" << endl;
}

int main() {
    // stringstream input("13\nStop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\nStop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\nBus number 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\nBus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\nStop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino\nStop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\nStop Biryusinka: 55.581065, 37.64839, 750m to Universam\nStop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\nStop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\nStop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\nBus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\nStop Rossoshanskaya ulitsa: 55.595579, 37.605757\nStop Prazhskaya: 55.611678, 37.603831\n");
    // stringstream in("6\nBus number 256\nBus 750\nBus 751\nStop Samara\nStop Prazhskaya\nStop Biryulyovo Zapadnoye\n");
    // TransportCatalogue cat(input);
    //stringstream i("1\nBus number 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n");
   // cat.AddRoute(i);
   // cout << cat.GetRoutInfo(in);
    // istringstream input("4\nStop A: 0.5, -1, 100000m to B\nStop B: 0, -1.1\nStop C: -2, -1.1\nBus 256: B - A\n");
    // TransportCatalogue cat(input);
    // istringstream in("1\nBus 256\n");
    // cout << cat.GetRoutInfo(in);
    TransportCatalogue c(cin);
    cout << c.GetRoutInfo(cin);
    //Test();

    return 0;
}
/*
Задание 1
Транспортный справочник. Часть A
В этом задании нужно реализовать систему хранения транспортных маршрутов и обработки запросов к ней. Сначала на вход подаются запросы на создание базы данных, затем — запросы к самой базе.
При проектировании решения учитывайте, что его предстоит неоднократно расширять. Авторские решения вы увидите после успешной сдачи всех версий. Таким образом, внося все доработки в функционал, вы будете иметь дело с собственной архитектурой кода и почувствуете, какие именно проектировочные решения мешают расширению.
Разбейте задачу на файлы. Реализуйте чтение в отдельном модуле, который будет отвечать за ввод и вывод данных. Сам транспортный справочник реализуйте в отдельном классе, который не работает со вводом и выводом. Также создайте структуры для хранения информации об остановке и об автобусном маршруте.
Формат ввода базы данных
В первой строке стандартного потока ввода содержится число N — количество запросов на обновление базы данных, затем — по одному на строке — вводятся сами запросы. Запросы бывают двух типов.
Stop X: latitude, longitude
Добавляет информацию об остановке с названием X и координатами latitude (широта) и longitude (долгота) на земной поверхности. Название остановки может состоять из нескольких слов. Используйте двоеточие как признак окончания названия остановки. Широта задаётся в градусах от -90.0 (южный полюс) до +90.0 (северный полюс). Положительные широты расположены севернее экватора, отрицательные — южнее. Долгота задаётся в градусах от -180.0 до +180.0, положительные значения соответствуют восточной долготе, а отрицательные — западной. Нулевой меридиан проходит через Гринвичскую королевскую обсерваторию в Лондоне, а координаты останкинской телевышки равны 55.8199081 северной широты и 37.6116028 восточной долготы. Широта и долгота разделяются запятой, за которой следует пробел.
Гарантируется, что остановка X определена не более чем в одном запросе Stop.
Пример объявления остановки:

Stop Biryulyovo Zapadnoye: 55.574371, 37.651700

Bus X: описание маршрута
Запрос на добавление автобусного маршрута X. Название маршрута может состоять из нескольких слов и отделяется от описания символом двоеточия. Описание маршрута может задаваться в одном из двух форматов (см. пример):

    stop1 - stop2 - ... stopN: автобус следует от stop1 до stopN и обратно с указанными промежуточными остановками.

    image

    Маршрут, заданный остановками A, B, C, D. Автобус проходит через остановки: A, B, C, D, C, B, A
    stop1 > stop2 > ... > stopN > stop1: кольцевой маршрут с конечной stop1.

image

Кольцевой маршрут, заданный остановками A, B, C, D, A
По сути первая версия описания маршрута эквивалентна stop1 > stop2 > ... > stopN-1 > stopN > stopN-1 > ... > stop2 > stop1.
На рисунке показаны шесть остановок: A, B, C, D, E, F и два автобусных маршрута: кольцевой A-C-D-B-A и обычный E>B>D>F.
image

Маршруты A>C>D>B>A и E-B-D-F
Гарантируется, что каждая из остановок маршрута определена в некотором запросе Stop, а сам маршрут X определён не более чем в одном запросе Bus.
Формат запросов к базе данных
Запросы к базе данных подаются в cin после запросов на создание базы. В первой строке вводится количество запросов, затем — по одному в строке — вводятся сами запросы. Запросы в этой задаче бывают только одного типа.
Bus X
Вывести информацию об автобусном маршруте X в следующем формате:
Bus X: R stops on route, U unique stops, L route length
Здесь

    X — название маршрута. Оно совпадает с названием, переданным в запрос Bus.
    R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
    U — количество уникальных остановок, на которых останавливается автобус. Одинаковыми считаются остановки, имеющие одинаковые названия.
    L — длина маршрута в метрах. В этом задании для простоты считается, что автобус проезжает путь между двумя соседними остановками по кратчайшему расстоянию по земной поверхности. Для вычисления расстояния между двумя точками пользуйтесь функцией Computegeo_lengthance из заготовки кода.

Величину расстояния, а также другие вещественные числа в последующих частях задачи, выводите с шестью значащими цифрами, то есть предваряя манипулятором setprecision(6).
Если маршрут X не найден, выведите Bus X: not found.
Ограничения
Код должен быть аккуратным, эффективным, структурированным и выполнять свою задачу.
После следующей темы код транспортного справочника будет проходить ревью.
Разместите код программы в следующих исходных файлах:

    transport_catalogue.h, transport_catalogue.cpp — класс транспортного справочника;
    geo.h — функции для работы с географическими координатами;
    input_reader.h, input_reader.cpp — чтение запросов на заполнение базы;
    stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;
    main.cpp — точка входа.

Класс транспортного справочника назовите TransportCatalogue. Он должен иметь методы для выполнения следующих задач:

    добавление маршрута в базу,
    добавление остановки в базу,
    поиск маршрута по имени,
    поиск остановки по имени,
    получение информации о маршруте.

Методы классаTransportCatalogue не должны выполнять никакого ввода-вывода. В будущих версиях программы формат входных и выходных данных программы будет изменён. Отделение логики от ввода-вывода позволит легко изменить формат выходных и выходных данных, не затрагивая логику приложения.
Эффективность
При разработке следует уделять внимание эффективности кода. Придерживайтесь ограничений, перечисленных ниже.

    Сложность методов и функций должна быть оптимальной:

        чтение запроса производится за время O(N)O(N), где NN — количество символов в нём,
        добавление остановки и маршрута производится со сложностью амортизированная O(K)O(K) в среднем, где KK — длина названия,
        нахождение остановки или маршрута по их названию в среднем — амортизированная O(K)O(K), где KK — длина названия,
        получение информации о маршруте производится со сложностью амортизированная  O(1)O(1) в среднем.
    Используйте, где возможно, перемещение строк и других больших объектов вместо копирования.
    Не храните несколько копий одних и тех же данных. Например, информация об остановке не должна дублироваться в маршрутах, которые проходят через эту остановку. Придумайте, как в качестве ключей ассоциативных контейнеров вместо string использовать string_view, ссылающихся на название маршрута или остановки.

Как будет тестироваться ваша программа
Допустимые символы в названиях маршрутов и остановок — латинские буквы, цифры и пробелы. Все названия непусты, не могут начинаться на пробелы или заканчиваться ими.
Гарантируется, что тренажёр будет подавать только корректные входные данные, которые соответствуют описанному формату.
Пример
Ввод

10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751


Вывод

Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found


13
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831buses_.push_back
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye

Вывод:
Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828


13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye

Вывод
Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828

 */