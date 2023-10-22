#include "json_builder.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include <sstream>
#include <fstream>
#include <iostream>


using namespace std;

int main() {
  json::Print(
    json::Document{
        json::Builder{}
        .StartDict()
            .Key("key0").Value(1.03e+6)
            .Key("key1"s).Value(123. / 34558.)
            .Key("key2"s).Value("value2"s)
            .Key("key3").Value(42.48356803246797)
            .Key("key4"s).StartArray()
                .Value(456)
                .StartDict().EndDict()
                .StartDict()
                    .Key(""s)
                    .Value(nullptr)
                .EndDict()
                .Value(""s)
            .EndArray()
        .EndDict()
        .Build()
    },
    cout
  );
  cout << endl;

  json::Print(
    json::Document{
        json::Builder{}
        .Value("just a string"s)
        .Build()
    },
    cout
  );
  cout << endl;
  TransportCatalogue cat;
  JSONReader json_reader(cin);
  json_reader.FillCatalogue(cat);
  RequestHandler handler(cat, HandleRenderSettings(json_reader.GetRenderSettings()));
  //handler.RenderMap().Render(std::cout);
  json::Print(json::Document(json_reader.HandleStatRequests(handler)), std::cout);
}