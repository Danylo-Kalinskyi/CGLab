#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera()
{
	view_matrix.SetIdentity();
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
    Matrix44 R;
    R.MakeRotationMatrix(angle, axis);
    Vector3 distance_vec = eye - center;
    Vector3 rotated_dist = R.RotateVector(distance_vec);
    eye = center + rotated_dist;
    UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    view_matrix.SetIdentity();
    Vector3 f = (center - eye);
    f.Normalize();
    Vector3 world_up(0, 1, 0);
    Vector3 s = f.Cross(world_up); 
    
    if (s.Length() < 0.0001f) {
        s = Vector3(1, 0, 0); // Default to X-axis
    }
    s.Normalize();
    Vector3 u = s.Cross(f);

    view_matrix.M[0][0] = s.x;  view_matrix.M[1][0] = s.y;  view_matrix.M[2][0] = s.z;
    view_matrix.M[0][1] = u.x;  view_matrix.M[1][1] = u.y;  view_matrix.M[2][1] = u.z;
    view_matrix.M[0][2] = -f.x; view_matrix.M[1][2] = -f.y; view_matrix.M[2][2] = -f.z;

    // Apply Translation
    Matrix44 T;
    T.MakeTranslationMatrix(-eye.x, -eye.y, -eye.z);
    
    view_matrix = view_matrix * T;

    UpdateViewProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
    projection_matrix.SetIdentity();

    if (type == PERSPECTIVE) {
        float f = 1.0f / tan((fov * (M_PI / 180.0f)) / 2.0f);
        
        projection_matrix.M[0][0] = f / aspect;
        projection_matrix.M[1][1] = f;
        projection_matrix.M[2][2] = (far_plane + near_plane) / (near_plane - far_plane);
        projection_matrix.M[2][3] = -1.0f;
        projection_matrix.M[3][2] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);
        projection_matrix.M[3][3] = 0.0f;
    }
    else if (type == ORTHOGRAPHIC) {
        projection_matrix.M[0][0] = 2.0f / (right - left);
        projection_matrix.M[1][1] = 2.0f / (top - bottom);
        projection_matrix.M[2][2] = -2.0f / (far_plane - near_plane);
        
        projection_matrix.M[3][0] = -(right + left) / (right - left);
        projection_matrix.M[3][1] = -(top + bottom) / (top - bottom);
        projection_matrix.M[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    } 

    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}
