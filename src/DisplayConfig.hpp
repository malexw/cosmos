#ifndef COSMOS_DISPLAY_CONFIG_HPP_
#define COSMOS_DISPLAY_CONFIG_HPP_

#include <fstream>
#include <string>

struct DisplayConfig {
    bool windowed_fullscreen = true;
    int width = 0;
    int height = 0;

    static DisplayConfig load(const std::string& path) {
        DisplayConfig cfg;

        std::ifstream file(path);
        if (!file.is_open()) return cfg;

        bool in_display = false;
        std::string line;
        while (std::getline(file, line)) {
            // trim
            size_t start = line.find_first_not_of(" \t");
            if (start == std::string::npos) continue;
            size_t end = line.find_last_not_of(" \t");
            line = line.substr(start, end - start + 1);

            if (line.empty() || line[0] == '#') continue;

            // section header
            if (line.front() == '[' && line.back() == ']') {
                std::string section = line.substr(1, line.size() - 2);
                // trim section name
                size_t s = section.find_first_not_of(" \t");
                size_t e = section.find_last_not_of(" \t");
                if (s != std::string::npos)
                    section = section.substr(s, e - s + 1);
                in_display = (section == "display");
                continue;
            }

            if (!in_display) continue;

            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);

            // trim key
            {
                size_t s = key.find_first_not_of(" \t");
                size_t e = key.find_last_not_of(" \t");
                if (s != std::string::npos)
                    key = key.substr(s, e - s + 1);
            }
            // trim value
            {
                size_t s = val.find_first_not_of(" \t");
                size_t e = val.find_last_not_of(" \t");
                if (s != std::string::npos)
                    val = val.substr(s, e - s + 1);
            }
            // strip quotes
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.size() - 2);
            }

            if (key == "mode" && val == "windowed") {
                cfg.windowed_fullscreen = false;
            } else if (key == "width") {
                cfg.width = std::stoi(val);
            } else if (key == "height") {
                cfg.height = std::stoi(val);
            }
        }

        return cfg;
    }
};

#endif
