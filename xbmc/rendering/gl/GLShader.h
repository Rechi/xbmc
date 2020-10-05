/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/Shader.h"

#include <string>

class CGLShader : public Shaders::CGLSLShaderProgram
{
public:
  CGLShader(const char *shader, std::string prefix);
  CGLShader(const char *vshader, const char *fshader, std::string prefix);
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;
  void Free();

  GLint GetPosLoc() const { return m_hPos; }
  GLint GetColLoc() const { return m_hCol; }
  GLint GetCord0Loc() const { return m_hCord0; }
  GLint GetCord1Loc() const { return m_hCord1; }
  GLint GetUniColLoc() const { return m_hUniCol; }
  GLint GetModelLoc() const { return m_hModel; }
  bool HardwareClipIsPossible() const { return m_clipPossible; }
  GLfloat GetClipXFactor() const { return m_clipXFactor; }
  GLfloat GetClipXOffset() const { return m_clipXOffset; }
  GLfloat GetClipYFactor() const { return m_clipYFactor; }
  GLfloat GetClipYOffset() const { return m_clipYOffset; }

protected:
  GLint m_hTex0 = 0;
  GLint m_hTex1 = 0;
  GLint m_hUniCol = 0;
  GLint m_hProj = 0;
  GLint m_hModel = 0;
  GLint m_hPos = 0;
  GLint m_hCol = 0;
  GLint m_hCord0 = 0;
  GLint m_hCord1 = 0;

  const GLfloat *m_proj = nullptr;
  const GLfloat *m_model = nullptr;

  bool m_clipPossible = false;
  GLfloat m_clipXFactor;
  GLfloat m_clipXOffset;
  GLfloat m_clipYFactor;
  GLfloat m_clipYOffset;
};
