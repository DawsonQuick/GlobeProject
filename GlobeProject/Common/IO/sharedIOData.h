#pragma once
#pragma optimize("", off)
// GLFW
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>
#include <string>
#include <map>


// Forward declaration of the Camera class
class Camera;


/*
* This section of the IO Namespace it so hold data that is used throught the IO namespace to make it accessible in a centralized location
*/
namespace IO {



    //Prototype
    inline std::string getKeyName(int key);
    struct KeyProcessor;

    //TODO: figure out how to make private to IO namespace
    inline bool isActiveCameraSet = false;
    inline Camera* activeCamera = nullptr;
    inline std::map<std::string,KeyProcessor> keyProcessors;
    inline std::map<std::string, std::string> nameToKeyStringRepresentation;
    inline float deltaTime;
    inline float lastFrameTime;
    
    enum KeyDetectionAction {
        OnPress,
        OnHold
    };

    enum InputType {
        KeyboardInput,
        MouseButtonInput
    };

    struct KeyProcessor {
        std::map<int, bool> wasKeyPressed;
        std::vector<int> inputIds;  // Can be key IDs or mouse button IDs
        std::function<void()> action;
        KeyDetectionAction detectionAction;  // Detection action (OnPress or OnHold)
        InputType inputType;  // Type of input (keyboard or mouse button)
        bool overrideImGuiFocused;

        bool isTriggered = false;  // Tracks if the action has already been triggered

        KeyProcessor() {}

        KeyProcessor(std::vector<int> tmpInputIds, std::function<void()> action, KeyDetectionAction detectionAction, InputType inputType , bool overrideImGuiFocused)
            : inputIds(std::move(tmpInputIds)), action(std::move(action)), detectionAction(detectionAction), inputType(inputType), overrideImGuiFocused(overrideImGuiFocused){
            for (int id : inputIds) {
                wasKeyPressed[id] = false;
            }
        }

        bool process(GLFWwindow* window) {
            bool allInputsPressed = true;

            // Check the current state of all inputs (keys or mouse buttons)
            for (int id : inputIds) {
                int inputState = (inputType == KeyboardInput)
                    ? glfwGetKey(window, id)
                    : glfwGetMouseButton(window, id);

                if (inputState != GLFW_PRESS) {
                    allInputsPressed = false;
                    break;
                }
            }

            if (detectionAction == OnPress) {
                // OnPress logic: Trigger the action once when all inputs are pressed
                if (allInputsPressed && !isTriggered) {
                    action();  // Trigger the action once
                    isTriggered = true;  // Mark the action as triggered
                }
                else if (!allInputsPressed) {
                    isTriggered = false;  // Reset the trigger when not all inputs are pressed
                }
            }
            else if (detectionAction == OnHold) {
                // OnHold logic: Trigger the action continuously while all inputs are pressed
                if (allInputsPressed) {
                    action();  // Trigger the action continuously while holding
                    isTriggered = true;
                }
                else {
                    isTriggered = false;  // Stop triggering if any input is released
                }
            }

            return isTriggered;
        }
    };


    inline std::string getKeyName(int key) {
        switch (key) {
        case GLFW_KEY_SPACE: return "SPACE";
        case GLFW_KEY_APOSTROPHE: return "APOSTROPHE";
        case GLFW_KEY_COMMA: return "COMMA";
        case GLFW_KEY_MINUS: return "MINUS";
        case GLFW_KEY_PERIOD: return "PERIOD";
        case GLFW_KEY_SLASH: return "SLASH";
        case GLFW_KEY_0: return "0";
        case GLFW_KEY_1: return "1";
        case GLFW_KEY_2: return "2";
        case GLFW_KEY_3: return "3";
        case GLFW_KEY_4: return "4";
        case GLFW_KEY_5: return "5";
        case GLFW_KEY_6: return "6";
        case GLFW_KEY_7: return "7";
        case GLFW_KEY_8: return "8";
        case GLFW_KEY_9: return "9";
        case GLFW_KEY_SEMICOLON: return "SEMICOLON";
        case GLFW_KEY_EQUAL: return "EQUAL";
        case GLFW_KEY_A: return "A";
        case GLFW_KEY_B: return "B";
        case GLFW_KEY_C: return "C";
        case GLFW_KEY_D: return "D";
        case GLFW_KEY_E: return "E";
        case GLFW_KEY_F: return "F";
        case GLFW_KEY_G: return "G";
        case GLFW_KEY_H: return "H";
        case GLFW_KEY_I: return "I";
        case GLFW_KEY_J: return "J";
        case GLFW_KEY_K: return "K";
        case GLFW_KEY_L: return "L";
        case GLFW_KEY_M: return "M";
        case GLFW_KEY_N: return "N";
        case GLFW_KEY_O: return "O";
        case GLFW_KEY_P: return "P";
        case GLFW_KEY_Q: return "Q";
        case GLFW_KEY_R: return "R";
        case GLFW_KEY_S: return "S";
        case GLFW_KEY_T: return "T";
        case GLFW_KEY_U: return "U";
        case GLFW_KEY_V: return "V";
        case GLFW_KEY_W: return "W";
        case GLFW_KEY_X: return "X";
        case GLFW_KEY_Y: return "Y";
        case GLFW_KEY_Z: return "Z";
        case GLFW_KEY_LEFT_BRACKET: return "LEFT_BRACKET";
        case GLFW_KEY_BACKSLASH: return "BACKSLASH";
        case GLFW_KEY_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case GLFW_KEY_GRAVE_ACCENT: return "GRAVE_ACCENT";
        case GLFW_KEY_WORLD_1: return "WORLD_1";
        case GLFW_KEY_WORLD_2: return "WORLD_2";
        case GLFW_KEY_ESCAPE: return "ESCAPE";
        case GLFW_KEY_ENTER: return "ENTER";
        case GLFW_KEY_TAB: return "TAB";
        case GLFW_KEY_BACKSPACE: return "BACKSPACE";
        case GLFW_KEY_INSERT: return "INSERT";
        case GLFW_KEY_DELETE: return "DELETE";
        case GLFW_KEY_RIGHT: return "RIGHT";
        case GLFW_KEY_LEFT: return "LEFT";
        case GLFW_KEY_DOWN: return "DOWN";
        case GLFW_KEY_UP: return "UP";
        case GLFW_KEY_PAGE_UP: return "PAGE_UP";
        case GLFW_KEY_PAGE_DOWN: return "PAGE_DOWN";
        case GLFW_KEY_HOME: return "HOME";
        case GLFW_KEY_END: return "END";
        case GLFW_KEY_CAPS_LOCK: return "CAPS_LOCK";
        case GLFW_KEY_SCROLL_LOCK: return "SCROLL_LOCK";
        case GLFW_KEY_NUM_LOCK: return "NUM_LOCK";
        case GLFW_KEY_PRINT_SCREEN: return "PRINT_SCREEN";
        case GLFW_KEY_PAUSE: return "PAUSE";
        case GLFW_KEY_F1: return "F1";
        case GLFW_KEY_F2: return "F2";
        case GLFW_KEY_F3: return "F3";
        case GLFW_KEY_F4: return "F4";
        case GLFW_KEY_F5: return "F5";
        case GLFW_KEY_F6: return "F6";
        case GLFW_KEY_F7: return "F7";
        case GLFW_KEY_F8: return "F8";
        case GLFW_KEY_F9: return "F9";
        case GLFW_KEY_F10: return "F10";
        case GLFW_KEY_F11: return "F11";
        case GLFW_KEY_F12: return "F12";
        case GLFW_KEY_F13: return "F13";
        case GLFW_KEY_F14: return "F14";
        case GLFW_KEY_F15: return "F15";
        case GLFW_KEY_F16: return "F16";
        case GLFW_KEY_F17: return "F17";
        case GLFW_KEY_F18: return "F18";
        case GLFW_KEY_F19: return "F19";
        case GLFW_KEY_F20: return "F20";
        case GLFW_KEY_F21: return "F21";
        case GLFW_KEY_F22: return "F22";
        case GLFW_KEY_F23: return "F23";
        case GLFW_KEY_F24: return "F24";
        case GLFW_KEY_F25: return "F25";
        case GLFW_KEY_KP_0: return "KP_0";
        case GLFW_KEY_KP_1: return "KP_1";
        case GLFW_KEY_KP_2: return "KP_2";
        case GLFW_KEY_KP_3: return "KP_3";
        case GLFW_KEY_KP_4: return "KP_4";
        case GLFW_KEY_KP_5: return "KP_5";
        case GLFW_KEY_KP_6: return "KP_6";
        case GLFW_KEY_KP_7: return "KP_7";
        case GLFW_KEY_KP_8: return "KP_8";
        case GLFW_KEY_KP_9: return "KP_9";
        case GLFW_KEY_KP_DECIMAL: return "KP_DECIMAL";
        case GLFW_KEY_KP_DIVIDE: return "KP_DIVIDE";
        case GLFW_KEY_KP_MULTIPLY: return "KP_MULTIPLY";
        case GLFW_KEY_KP_SUBTRACT: return "KP_SUBTRACT";
        case GLFW_KEY_KP_ADD: return "KP_ADD";
        case GLFW_KEY_KP_ENTER: return "KP_ENTER";
        case GLFW_KEY_KP_EQUAL: return "KP_EQUAL";
        case GLFW_KEY_LEFT_SHIFT: return "LEFT_SHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LEFT_CONTROL";
        case GLFW_KEY_LEFT_ALT: return "LEFT_ALT";
        case GLFW_KEY_LEFT_SUPER: return "LEFT_SUPER";
        case GLFW_KEY_RIGHT_SHIFT: return "RIGHT_SHIFT";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
        case GLFW_KEY_RIGHT_ALT: return "RIGHT_ALT";
        case GLFW_KEY_RIGHT_SUPER: return "RIGHT_SUPER";
        case GLFW_KEY_MENU: return "MENU";
        default: return "UNKNOWN";
        }
    }

}
#pragma optimize("", on)