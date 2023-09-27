#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>
#include <sstream>


int main() {
  using namespace std;
  istringstream iss(R"({"render_settings": {"width": 18603.015162861815, "height": 94867.02060408464, "padding": 6720.349966650843, "stop_radius": 51532.613280783466, "line_width": 52242.62154924796, "stop_label_font_size": 2558, "stop_label_offset": [-92354.66283480934, 14798.889154458797], "underlayer_color": [52, 240, 58, 0.230923061046871], "underlayer_width": 41136.846431128295, "color_palette": ["thistle", "plum", [239, 176, 24], [3, 133, 76, 0.397629865747683], [93, 31, 167], "orchid", [108, 88, 199], "orchid", "fuchsia", [202, 43, 121, 0.9499584145277128], "navy", "pink", "lime", "tomato", [194, 52, 227], [224, 136, 254], [111, 225, 55, 0.9112604581273795], [201, 187, 3, 0.01430675933678327], [51, 144, 56, 0.4518442205932798], [195, 109, 57, 0.24519016253008008], "tomato", "maroon", [180, 126, 32], [233, 252, 78], "teal", "navy", "black", [116, 81, 220, 0.6385215767191011], "white", [110, 209, 113], [236, 134, 13], [124, 228, 145], [139, 147, 250], [57, 226, 51], [56, 138, 37, 0.11077753244715827], [85, 130, 171], "orange", [63, 44, 41, 0.3198577275225444], "brown", [72, 246, 203], [87, 145, 226], "lime", [29, 184, 91], "green", "fuchsia", [23, 79, 112], "tomato", "bisque", "indigo", [135, 233, 24, 0.10720163623057166], [64, 83, 135, 0.12153770011199672], [185, 31, 176], [62, 27, 219], "black", "peru", "cyan", [14, 183, 25], [173, 115, 144], [242, 19, 175], [56, 179, 90], [144, 0, 150, 0.7473177316483276], [39, 187, 210], [194, 223, 100, 0.9728532923817715], [43, 85, 183], [239, 106, 210, 0.39850531051986915], [227, 51, 69], "cyan", "brown", [148, 62, 90, 0.24724931714033982], "cyan", [210, 121, 117], "gray", [96, 53, 50], "peru", "fuchsia", "cyan", [25, 53, 25], "lavender", "navy", [67, 229, 120], "maroon", [20, 42, 130, 0.15359390921821248], "thistle", [37, 117, 113], "peru", "blue", "tan", [226, 65, 82], [75, 36, 231], "khaki", [186, 215, 189], [48, 252, 179], [113, 16, 86, 0.37161833669224364], [106, 241, 95], [29, 153, 130, 0.6377717025904377], [109, 5, 234], "ivory"], "bus_label_font_size": 68836, "bus_label_offset": [45314.327764781745, 472.4338292726461]}, "base_requests": [{"type": "Stop", "name": "A1", "latitude": 55.611087, "longitude": 37.20829, "road_distances": {"A2": 1000, "A3": 1000}}, {"type": "Stop", "name": "A2", "latitude": 55.595884, "longitude": 37.209755, "road_distances": {"A3": 1000}}, {"type": "Stop", "name": "A3", "latitude": 55.632761, "longitude": 37.333324, "road_distances": {}}, {"type": "Bus", "name": "B1", "stops": ["A3", "A2"], "is_roundtrip": false}, {"type": "Bus", "name": "B2", "stops": ["A2", "A3"], "is_roundtrip": false}], "stat_requests": [{"id": 1919150282, "type": "Stop", "name": "A1"}]}
)");
  istringstream iss1(R"({
    "base_requests": [
      {
        "type": "Bus",
        "name": "14",
        "stops": [
          "Ulitsa Lizy Chaikinoi",
          "Elektroseti",
          "Ulitsa Dokuchaeva",
          "Ulitsa Lizy Chaikinoi"
        ],
        "is_roundtrip": true
      },
      {
        "type": "Bus",
        "name": "114",
        "stops": [
          "Morskoy vokzal",
          "Rivierskiy most"
        ],
        "is_roundtrip": false
      },
      {
        "type": "Stop",
        "name": "Rivierskiy most",
        "latitude": 43.587795,
        "longitude": 39.716901,
        "road_distances": {
          "Morskoy vokzal": 850
        }
      },
      {
        "type": "Stop",
        "name": "Morskoy vokzal",
        "latitude": 43.581969,
        "longitude": 39.719848,
        "road_distances": {
          "Rivierskiy most": 850
        }
      },
      {
        "type": "Stop",
        "name": "Elektroseti",
        "latitude": 43.598701,
        "longitude": 39.730623,
        "road_distances": {
          "Ulitsa Dokuchaeva": 3000,
          "Ulitsa Lizy Chaikinoi": 4300
        }
      },
      {
        "type": "Stop",
        "name": "Ulitsa Dokuchaeva",
        "latitude": 43.585586,
        "longitude": 39.733879,
        "road_distances": {
          "Ulitsa Lizy Chaikinoi": 2000,
          "Elektroseti": 3000
        }
      },
      {
        "type": "Stop",
        "name": "Ulitsa Lizy Chaikinoi",
        "latitude": 43.590317,
        "longitude": 39.746833,
        "road_distances": {
          "Elektroseti": 4300,
          "Ulitsa Dokuchaeva": 2000
        }
      }
    ],
    "render_settings": {
      "width": 600,
      "height": 400,
      "padding": 50,
      "stop_radius": 5,
      "line_width": 14,
      "bus_label_font_size": 20,
      "bus_label_offset": [
        7,
        15
      ],
      "stop_label_font_size": 20,
      "stop_label_offset": [
        7,
        -3
      ],
      "underlayer_color": [
        255,
        255,
        255,
        0.85
      ],
      "underlayer_width": 3,
      "color_palette": [
        "green",
        [255, 160, 0],
        "red"
      ]
    },
    "stat_requests": [
    ]
  }
)"s);
  istringstream iss2(R"({"render_settings": {"width": 89298.28369209476, "height": 58011.29248202205, "padding": 22325.567226490493, "stop_radius": 21462.68635533567, "line_width": 38727.57356370373, "stop_label_font_size": 86988, "stop_label_offset": [-23192.03299796056, 92100.21839665441], "underlayer_color": "coral", "underlayer_width": 34006.680510317055, "color_palette": [[195, 60, 81, 0.6244132141059138], [2, 81, 213], [81, 152, 19, 0.6834377639654173], [94, 70, 16, 0.7604371566734329], [191, 220, 104], "brown", [164, 135, 29], [224, 79, 33], [60, 83, 79], [180, 239, 251, 0.10891745780969919], "thistle", "green", [33, 11, 167], [109, 4, 131, 0.9177069334241762], [247, 229, 13, 0.3216590299988016], "green", [95, 179, 198], [91, 176, 239], "peru", "chocolate", [26, 29, 136], "orange", "gray", "red", "khaki", "wheat", [227, 109, 210, 0.48281657728221594], [241, 161, 45], "sienna", "orange", "purple", [72, 249, 41, 0.815095041128292], "black", [217, 166, 56, 0.5710638920827465], [41, 106, 150], "teal", [189, 73, 65], "magenta", [45, 221, 209], "brown", [32, 66, 192, 0.007171850904110877], "maroon", [109, 44, 208], [83, 94, 186], "red", "tan", [171, 253, 129], "olive", [222, 53, 126], [41, 124, 196], [67, 197, 163], "navy", "gold", [209, 141, 148, 0.13808356551454504], [207, 5, 255, 0.23583697757948507]], "bus_label_font_size": 78497, "bus_label_offset": [59718.916265509615, 15913.541281271406]}, "base_requests": [{"type": "Bus", "name": "i", "stops": ["gtKSYiTpuO3KjmLenbqOj7iO", "vtkKOKMLWRQv"], "is_roundtrip": false}, {"type": "Bus", "name": "jUYheoA3Mcm2 kqTN", "stops": ["gtKSYiTpuO3KjmLenbqOj7iO", "vtkKOKMLWRQv", "gtKSYiTpuO3KjmLenbqOj7iO"], "is_roundtrip": true}, {"type": "Stop", "name": "gtKSYiTpuO3KjmLenbqOj7iO", "latitude": 42.60758584742835, "longitude": 38.503607612387185, "road_distances": {"vtkKOKMLWRQv": 451128}}, {"type": "Stop", "name": "vtkKOKMLWRQv", "latitude": 39.663496626279645, "longitude": 39.296652056808824, "road_distances": {"gtKSYiTpuO3KjmLenbqOj7iO": 385612}}, {"type": "Bus", "name": "RkYZl", "stops": ["vtkKOKMLWRQv", "gtKSYiTpuO3KjmLenbqOj7iO"], "is_roundtrip": false}], "stat_requests": [{"id": 1658922989, "type": "Bus", "name": "RkYZl"}, {"id": 946024612, "type": "Bus", "name": "jUYheoA3Mcm2 kqTN"}, {"id": 1960236243, "type": "Bus", "name": "i"}, {"id": 946024612, "type": "Bus", "name": "jUYheoA3Mcm2 kqTN"}, {"id": 2067965337, "type": "Stop", "name": "vtkKOKMLWRQv"}, {"id": 1086415934, "type": "Stop", "name": "XK5ujSfpmS7p9mSx03uY"}, {"id": 967287382, "type": "Bus", "name": "xfRx1Ldg7LpeL9CMIL13A5q"}, {"id": 1425889316, "type": "Stop", "name": "gtKSYiTpuO3KjmLenbqOj7iO"}, {"id": 1658922989, "type": "Bus", "name": "RkYZl"}, {"id": 486321339, "type": "Map"}]}
)");
  TransportCatalogue cat;
  RequestsDataBase db = std::move(ReadJSON(iss2));
  FillCatalogue(cat, db.base_requests);
  RequestHandler handler(cat, HandleRenderSettings(db.render_settings));
  //handler.RenderMap().Render(std::cout);
  json::Print(json::Document(HandleStatRequests(handler, db.stat_requests)), std::cout);

  // MapRenderer map(HandleRenderSettings(db.render_settings));
  // map.MakeSVG(cat);

  //json::Print(json::Document(HandleStatRequests(cat, db.stat_requests)), std::cout);
  //json::Print(json::Document(json::LoadNode(iss)), std::cout);
  /*
   * Примерная структура программы:
   *
   * Считать JSON из stdin
   * Построить на его основе JSON базу данных транспортного справочника
   * Выполнить зпросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
   * с ответами.
   * Вывести в stdout ответы в виде JSON
   */
}