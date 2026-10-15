#include <iostream>
#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "math-objects.hpp"


void drawCurve(const Math::curve& c) {

    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);

    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& p : c.points) {
        glVertex3d(
            p.x,
            p.y,
            p.z
        );
    }

    glEnd();
}


void drawVector(
    const Math::Point& origin,
    const Math::Point& vector,
    float r,
    float g,
    float b,
    double scale = 1.0
) {

    glLineWidth(4.0f);

    glBegin(GL_LINES);

    glColor3f(r, g, b);

    glVertex3d(
        origin.x,
        origin.y,
        origin.z
    );

    glVertex3d(
        origin.x + vector.x * scale,
        origin.y + vector.y * scale,
        origin.z + vector.z * scale
    );

    glEnd();
}


int main() {

    // --------------------------------
    // GLFW
    // --------------------------------

    if (!glfwInit()) {
        std::cerr
            << "Failed to initialize GLFW\n";

        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(
        1000,
        800,
        "Differential Geometry Visualizer",
        nullptr,
        nullptr
    );

    if (!window) {

        std::cerr
            << "Failed to create GLFW window\n";

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);


    // --------------------------------
    // ImGui
    // --------------------------------

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(
        window,
        true
    );

    ImGui_ImplOpenGL3_Init(
        "#version 130"
    );


    // --------------------------------
    // Geometry parameters
    // --------------------------------

    float a = 2.0f;
    float b = 0.5f;

    float curveLength = 30.0f;

    float selectedT = 5.0f;

    bool parameterisedByArcLength = false;

    bool showTangent = true;
    bool showNormal = true;
    bool showBinormal = true;
    bool animate = false;
    float animationspeed = 1.0f;

    static int selectedCurve = 0;
    Math::curve Curve;


    // --------------------------------
    // Main loop
    // --------------------------------

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();


        // ================================
        // ImGui frame
        // ================================

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();


        // --------------------------------
        // Controls
        // --------------------------------

        ImGui::Begin("Controls");

        const char* curveNames[] = {
            "Helix",
            "Torus Knot",
            "Circle"
        };

        ImGui::Combo(
            "Select Curve",
            &selectedCurve,
            curveNames,
            IM_ARRAYSIZE(curveNames)
        );

        ImGui::Checkbox(
            "Parameterised by Arc Length",
            &parameterisedByArcLength
        );

        ImGui::Checkbox(
            "Animate",
            &animate
        );

        ImGui::SliderFloat(
            "Animation speed",
            &animationspeed,
            0.1f,
            5.0f
        );

        ImGui::SliderFloat(
            "Radius (a)",
            &a,
            0.1f,
            5.0f
        );

        ImGui::SliderFloat(
            "Pitch (b)",
            &b,
            0.0f,
            2.0f
        );

        ImGui::SliderFloat(
            "Curve length",
            &curveLength,
            5.0f,
            100.0f
        );

        ImGui::SliderFloat(
            parameterisedByArcLength
                ? "Position s"
                : "Position t",
            &selectedT,
            0.0f,
            curveLength
        );

        ImGui::Separator();

        ImGui::Checkbox(
            "Show Tangent",
            &showTangent
        );

        ImGui::Checkbox(
            "Show Normal",
            &showNormal
        );

        ImGui::Checkbox(
            "Show Binormal",
            &showBinormal
        );

        ImGui::End();

        if (animate) {
            selectedT += animationspeed * ImGui::GetIO().DeltaTime;

            if (selectedT >= curveLength) {
                selectedT = 0.0f;
            }
        }


        // --------------------------------
        // Generate geometry
        // --------------------------------

        const double step = 0.05;

        if (selectedCurve == 0) { // Helix
            if (parameterisedByArcLength) {
                Curve = Math::generateHelixArcLength(
                    a,
                    b,
                    0.0,
                    curveLength,
                    step
                );
            } else {
                Curve = Math::generateHelix(
                    a,
                    b,
                    0.0,
                    curveLength,
                    step
                );
            }
        } else if (selectedCurve == 1) { // Torus Knot
            Curve = Math::generateTorusKnot(
                a,
                b,
                2, // p
                3, // q
                0.0,
                curveLength,
                step
            );
        } else if (selectedCurve == 2) { // Circle
            Curve = Math::generateCircle(
                a,
                0.0,
                curveLength,
                step
            );
        }

        // Convert selected t into point index.

        std::size_t selectedIndex =
            static_cast<std::size_t>(
                selectedT / step
            );

        if (Curve.points.size() >= 4) {

            selectedIndex =
                std::min(
                    selectedIndex,
                    Curve.points.size() - 4
                );
        }

        double selectedCurvature =
        Math::curvature(
            Curve,
            selectedIndex
        );

    double selectedTorsion =
        Math::torsion(
            Curve,
            selectedIndex
        );


        // --------------------------------
        // Frenet frame
        // --------------------------------

        Math::frenet frame =
            Math::computeFrenetFrame(
                Curve,
                selectedIndex
            );

        const Math::Point& selectedPoint =
            Curve.points[selectedIndex];

        double pointSpacing =
            Math::distance(
                Curve.points[selectedIndex],
                Curve.points[selectedIndex + 1]
            );

        ImGui::Text(
            "Point spacing: %.5f",
            pointSpacing
        );

        ImGui::Text(
            "Parameter step: %.5f",
            step
        );


        // --------------------------------
        // Display numerical values
        // --------------------------------

        ImGui::Begin("Frenet Frame");

        ImGui::Text(
            "Point:    (%.3f, %.3f, %.3f)",
            selectedPoint.x,
            selectedPoint.y,
            selectedPoint.z
        );

        ImGui::Separator();

        ImGui::Text(
            "T: (%.3f, %.3f, %.3f)",
            frame.tangent.x,
            frame.tangent.y,
            frame.tangent.z
        );

        ImGui::Text(
            "N: (%.3f, %.3f, %.3f)",
            frame.normal.x,
            frame.normal.y,
            frame.normal.z
        );

        ImGui::Text(
            "B: (%.3f, %.3f, %.3f)",
            frame.binormal.x,
            frame.binormal.y,
            frame.binormal.z
        );

        ImGui::Text(
            "Curvature: %.5f",
            selectedCurvature
        );

        ImGui::Text(
            "Torsion: %.5f",
            selectedTorsion
        );

        ImGui::End();


        // ================================
        // OpenGL
        // ================================

        int width;
        int height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        glViewport(
            0,
            0,
            width,
            height
        );

        glClearColor(
            0.05f,
            0.05f,
            0.08f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        glEnable(GL_DEPTH_TEST);


        // --------------------------------
        // Simple camera
        // --------------------------------

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        const double aspect =
            static_cast<double>(width) /
            static_cast<double>(height);

        glOrtho(
            -6.0 * aspect,
             6.0 * aspect,
            -6.0,
             6.0,
            -20.0,
             20.0
        );


        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();


        // Rotate the scene so that we can
        // actually see the helix in 3D.

        glRotated(
            60.0,
            1.0,
            0.0,
            0.0
        );

        glRotated(
            -30.0,
            0.0,
            0.0,
            1.0
        );


        // --------------------------------
        // Draw curve
        // --------------------------------

        drawCurve(
            Curve
        );


        // --------------------------------
        // Draw Frenet frame
        // --------------------------------

        const double vectorScale = 1.5;


        // T = red

        if (showTangent) {

            drawVector(
                selectedPoint,
                frame.tangent,
                1.0f,
                0.2f,
                0.2f,
                vectorScale
            );
        }


        // N = green

        if (showNormal) {

            drawVector(
                selectedPoint,
                frame.normal,
                0.2f,
                1.0f,
                0.2f,
                vectorScale
            );
        }


        // B = blue

        if (showBinormal) {

            drawVector(
                selectedPoint,
                frame.binormal,
                0.2f,
                0.4f,
                1.0f,
                vectorScale
            );
        }


        // ================================
        // Render ImGui
        // ================================

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        glfwSwapBuffers(window);
    }


    // --------------------------------
    // Cleanup
    // --------------------------------

    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}