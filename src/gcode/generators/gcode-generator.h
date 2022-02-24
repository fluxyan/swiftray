#pragma once

#include <sstream>
#include <gcode/generators/base-generator.h>
#include <settings/machine-settings.h>

/*
Basic GCode Generator for Grbl like machines.
*/
class GCodeGenerator : public BaseGenerator {
public:
  GCodeGenerator(const MachineSettings::MachineSet &machine) : BaseGenerator() {
    machine_width_ = machine.width;
    machine_height_ = machine.height;
    machine_origin_ = machine.origin;
  }

  /**
   * @brief Input position from canvas coordinate -> generate G-Code for machine coordinate
   * @param x "absolute" position in real world scale and canvas axis direction
   * @param y "absolute" position in real world scale and canvas axis direction
   * @param speed
   * @param power
   */
  void moveTo(float x, float y, float speed, float power) override {
    // 1. Handle the axis direction (convert from canvas to machine)
    switch (machine_origin_) {
      case MachineSettings::MachineSet::OriginType::RearRight:
        // Canvas x axis direction is opposite to machine coordinate
        x = machine_width_ - x;
        break;
      case MachineSettings::MachineSet::OriginType::FrontRight:
        // Canvas x, y axis directions are opposite to machine coordinate
        x = machine_width_ - x;
        y = machine_height_ - y;
        break;
      case MachineSettings::MachineSet::OriginType::RearLeft:
        // NORMAL canvas x, y axis directions are the same as machine coordinate
        break;
      case MachineSettings::MachineSet::OriginType::FrontLeft:
        // Canvas y axis direction is opposite to machine coordinate
        y = machine_height_ - y;
        break;
      default:
        break;
    }

    // 2. separate relative mode & absolute mode
    if (relative_mode_) {
      if (x != x_ || y != y_) {
        str_stream_ << "G1";
      }
      if (x != x_) {
        str_stream_ << "X" << round((x - x_) * 1000) / 1000;
        x_ = x;
      }
      if (y != y_) {
        str_stream_ << "Y" << round((y - y_) * 1000) / 1000;
        y_ = y;
      }
    } else {
      // Coordinate transform for different origin type
      if (x_ == x && y_ == y && speed_ == speed && power_ == power)
        return;
      str_stream_ << "G1";
      if (x_ != x) {
        str_stream_ << "X" << round(x * 1000) / 1000;
        x_ = x;
      }
      if (y_ != y) {
        str_stream_ << "Y" << round(y * 1000) / 1000;
        y_ = y;
      }
    }

    if (speed_ != speed) {
      str_stream_ << "F" << speed * 60; // mm/s to mm/min
      speed_ = speed;
    }

    if (power_ != power) {
      str_stream_ << "S" << power * 10; // mm/s to mm/min
      power_ = power;
    }
    str_stream_ << std::endl;
  }

  void setLaserPower(float power) override {
    str_stream_ << "M3S" << power * 10 << std::endl;
    power_ = power;
  }

  void turnOffLaser() override {
    str_stream_ << "M5" << std::endl;
    power_ = 0;
  }

  void turnOnLaser() override {
    str_stream_ << "M3S0" << std::endl;
    power_ = 1;
  }

  void turnOnLaserAdpatively() override {
    str_stream_ << "M4S0" << std::endl;
    power_ = 1;
  }

  void useAbsolutePositioning() override {
    relative_mode_ = false;
    str_stream_ << "G90" << std::endl;
  }

  void useRelativePositioning() override {
    // TODO (Support actual G91)
    relative_mode_ = true;
    str_stream_ << "G91" << std::endl;
  }

  void home() override {
    str_stream_ << "$H" << std::endl;
    x_ = y_ = 0;
  }

  void reset() override {
    BaseGenerator::reset();
    relative_mode_ = false;
    machine_width_ = 0;
    machine_height_ = 0;
    machine_origin_ = MachineSettings::MachineSet::OriginType::RearLeft;
  }

private:
  bool relative_mode_;
  int machine_width_;
  int machine_height_;
  MachineSettings::MachineSet::OriginType machine_origin_;
};
