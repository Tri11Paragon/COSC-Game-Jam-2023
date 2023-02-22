//
// Created by administrator on 22/02/23.
//
#include <window.h>
#include "blt/std/logging.h"
#include <blt/std/time.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <thread>

std::unordered_map<int, bool> keys {};
std::unordered_map<int, bool> mouse {};

std::unordered_map<int, bool> key_state {};
std::unordered_map<int, bool> mouse_state {};

std::vector<IKeyEventListener*> keyListeners;
std::vector<IMouseEventListener*> mouseListeners;

auto last_frame_time = blt::system::getCurrentTimeNanoseconds();
double frame_delta = 0;

void Window::handleInput() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                BLT_INFO("Goodbye!");
                std::exit(0);
                break;
            case SDL_KEYDOWN:
                for (auto& p : keyListeners)
                    p->keyPressed(static_cast<SDL_KeyCode>(event.key.keysym.sym));
                keys[event.key.keysym.sym] = true;
                key_state[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                for (auto& p : keyListeners)
                    p->keyReleased(static_cast<SDL_KeyCode>(event.key.keysym.sym));
                keys[event.key.keysym.sym] = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                for (auto& p : mouseListeners)
                    p->mousePressed(event.button.button, event.button.x, event.button.y);
                mouse[event.button.button] = true;
                mouse_state[event.button.button] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                for (auto& p : mouseListeners)
                    p->mouseReleased(event.button.button, event.button.x, event.button.y);
                mouse[event.button.button] = false;
                break;
            default:
                break;
        }
    }
}

bool Window::keyDown(SDL_Keycode key) {
    return keys[(int)key];
}

bool Window::keyState(SDL_Keycode key) {
    return key_state[(int)key];
}

bool Window::mouseDown(int button) {
    return mouse[button];
}

bool Window::mouseState(int button) {
    return mouse_state[button];
}

void Window::registerListener(IKeyEventListener* listener) {
    keyListeners.push_back(listener);
}

void Window::registerListener(IMouseEventListener* listener) {
    mouseListeners.push_back(listener);
}

void Window::deleteListener(IKeyEventListener* listener) {
    auto position = std::find(keyListeners.begin(), keyListeners.end(), listener);
    if (position != keyListeners.end())
        keyListeners.erase(position);
}

void Window::deleteListener(IMouseEventListener* listener) {
    auto position = std::find(mouseListeners.begin(), mouseListeners.end(), listener);
    if (position != mouseListeners.end())
        mouseListeners.erase(position);
}

void Window::prepare() const {
    SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
    SDL_RenderClear(renderer);
}

void Window::sync(double target) const {
    SDL_RenderPresent(renderer);
    
    for (const auto& item : key_state)
        key_state[item.first] = false;
    for (const auto& button : mouse_state)
        mouse_state[button.first] = false;
    
    auto current_frame_time = blt::system::getCurrentTimeNanoseconds();
    auto frame_difference = current_frame_time - last_frame_time;
    auto target_nanoseconds = (long)(target * 1000000);
    
    if (frame_difference < target_nanoseconds) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(target_nanoseconds - frame_difference));
        current_frame_time = blt::system::getCurrentTimeNanoseconds();
        frame_difference = current_frame_time - last_frame_time;
    }
    
    frame_delta = (double)frame_difference / 1000000.0;
    last_frame_time = current_frame_time;
}

double Window::delta() {
    return frame_delta;
}

double Window::deltaSeconds() {
    return frame_delta / 1000.0;
}
