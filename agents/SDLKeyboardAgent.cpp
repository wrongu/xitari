/* *****************************************************************************
 * Xitari
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and 
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  SDLKeyboardAgent.cpp
 *
 * The implementation of the SDLKeyboardAgent class.
 **************************************************************************** */

#include "SDLKeyboardAgent.hpp"
#include "common/random_tools.h"
#include <stdexcept>
#include <string>

#ifdef __USE_SDL

using namespace ale;
using std::string;
using std::cout;
using std::endl;

SDLKeyboardAgent::SDLKeyboardAgent(OSystem* _osystem, RomSettings* _settings) : 
    PlayerAgent(_osystem, _settings), manual_control(false), playback_speed(50) {
  Settings& settings = p_osystem->settings();

  // If not displaying the screen, there is little point in having keyboard control
  bool display_screen = settings.getBool("display_screen", false);
  if (display_screen) {
      p_osystem->p_display_screen->registerEventHandler(this);
  }
  else
    throw new std::invalid_argument("Keyboard agent needs display_screen = true.");

}

Action SDLKeyboardAgent::act() {
  if (manual_control) {
      return waitForKeypress();
  } 
  else // Default to random agent 
    return choice(&available_actions);
}


bool SDLKeyboardAgent::handleSDLEvent(const SDL_Event& event) {
    switch(event.type) {
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
        case SDLK_p:
            if (manual_control) {
                // Set the pause status to whatever it previously was
                p_osystem->p_display_screen->setPaused(returnToPause);
                cout << "Returning to Automatic Control." << endl;
            } else {
                // Capture the pause status and store
                returnToPause = p_osystem->p_display_screen->paused;
                cout << "ReturntoPause: " << returnToPause << endl;
                p_osystem->p_display_screen->setPaused(false);
                printf("Starting Manual Control. Commands are as follows:\n  -p: return to auto control\n  -arrow keys: joystick movement\n  -space: button/fire\n  -return: no-op\n");
            }
            manual_control = !manual_control;
            return true;
        default:
            // If a key is pressed while manual control is active, let the manual controller handle it
            if (manual_control) return true;
            break;
        }
    default:
        break;
    }
    return false;
}

void SDLKeyboardAgent::usage() {
    printf("  -p: Toggle manual control of the agent\n");
}

inline int max(int a, int b){
    return (a) > (b) ? a : b;
}

inline int min(int a, int b){
    return (a) < (b) ? a : b;
}

Action SDLKeyboardAgent::waitForKeypress() {
    Action a = UNDEFINED;
    // This loop is necessary because keypress events come in quickly
    while (a == UNDEFINED) {
        SDL_Delay(playback_speed); // Set amount of sleep time
        SDL_PumpEvents();
        Uint8* keymap = SDL_GetKeyState(NULL);

        // control playback speed
        if(keymap[SDLK_f]) // faster
            playback_speed = max(playback_speed - 5, 10);
        else if(keymap[SDLK_s]) // slower
            playback_speed = min(playback_speed + 5, 100);

        // Break out of this loop if the 'p' key is pressed
        if (keymap[SDLK_p]) {
            return PLAYER_A_NOOP;

            // Triple Actions
        } else if (keymap[SDLK_UP] && keymap[SDLK_RIGHT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_UPRIGHTFIRE;
        } else if (keymap[SDLK_UP] && keymap[SDLK_LEFT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_UPLEFTFIRE;
        } else if (keymap[SDLK_DOWN] && keymap[SDLK_RIGHT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_DOWNRIGHTFIRE;
        } else if (keymap[SDLK_DOWN] && keymap[SDLK_LEFT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_DOWNLEFTFIRE;

            // Double Actions
        } else if (keymap[SDLK_UP] && keymap[SDLK_LEFT]) {
            a = PLAYER_A_UPLEFT;
        } else if (keymap[SDLK_UP] && keymap[SDLK_RIGHT]) {
            a = PLAYER_A_UPRIGHT;
        } else if (keymap[SDLK_DOWN] && keymap[SDLK_LEFT]) {
            a = PLAYER_A_DOWNLEFT;
        } else if (keymap[SDLK_DOWN] && keymap[SDLK_RIGHT]) {
            a = PLAYER_A_DOWNRIGHT;
        } else if (keymap[SDLK_UP] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_UPFIRE;
        } else if (keymap[SDLK_DOWN] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_DOWNFIRE;
        } else if (keymap[SDLK_LEFT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_LEFTFIRE;
        } else if (keymap[SDLK_RIGHT] && keymap[SDLK_SPACE]) {
            a = PLAYER_A_RIGHTFIRE;

            // Single Actions
        } else if (keymap[SDLK_SPACE]) {
            a = PLAYER_A_FIRE;
        } else if (keymap[SDLK_RETURN]) {
            a = PLAYER_A_NOOP;
        } else if (keymap[SDLK_LEFT]) {
            a = PLAYER_A_LEFT;
        } else if (keymap[SDLK_RIGHT]) {
            a = PLAYER_A_RIGHT;
        } else if (keymap[SDLK_UP]) {
            a = PLAYER_A_UP;
        } else if (keymap[SDLK_DOWN]) {
            a = PLAYER_A_DOWN;
        } 
    }
    return a;
}
#endif

