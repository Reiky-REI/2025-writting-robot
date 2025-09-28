# 2025 Writing Robot

## Project Overview
$$
[(x - 0.2*sqrt(-(5.0*y - 5.0*y_C - 66.0)*(5.0*y - 5.0*y_C + 66.0)), y_C), (x + 0.2*sqrt(-(5.0*y - 5.0*y_C - 66.0)*(5.0*y - 5.0*y_C + 66.0)), y_C)]
\\
\left( x - 0.2 \sqrt{- \left(5.0 y - 5.0 y_{C} - 66.0\right) \left(5.0 y - 5.0 y_{C} + 66.0\right)}, \  y_{C}\right)

$$


The 2025 Writing Robot project aims to create a robotic system capable of writing characters based on user input. The system utilizes servo motors to control the writing mechanism, and it processes character input to generate precise movements for drawing.

## Project Structure
The project is organized into the following directories and files:

```
2025-writing-robot
├── code
│   └── Rpi
│       ├── main
│       │   ├── main.py               # Main entry point for the application
│       │   ├── inverse.py            # Contains inverse kinematics calculations
│       │   ├── _01_servo_A15_ST_LC_raspberry_lib_DR.py  # Library for controlling A15-ST servo motors
│       │   └── writing.py             # Implements character input and drawing functionality
│       └── front
│           ├── front.py               # Generates bitmap images of characters
│           ├── line.py                # Extracts skeletons from bitmap images
│           └── diraction.py           # Generates drawing instructions based on skeletons
└── README.md
```

## Setup Instructions
1. **Clone the Repository**: 
   Clone this repository to your local machine using:
   ```
   git clone <repository-url>
   ```

2. **Install Dependencies**: 
   Ensure you have the necessary libraries installed. You may need to install the following Python packages:
   ```
   pip install numpy opencv-python pillow
   ```

3. **Connect the Hardware**: 
   Connect the A15-ST servo motors to the Raspberry Pi as per the specifications provided in the servo library.

4. **Run the Application**: 
   Navigate to the `code/Rpi/main` directory and run the main application:
   ```
   python main.py
   ```

## Usage
- The application will prompt for a character input.
- It will generate a 32x32 bitmap of the character, analyze its skeleton, and generate a sequence of movements for the servo motors to draw the character.
- The movements will be executed with a delay of 0.6 seconds between each step.

## Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.