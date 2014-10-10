/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include <QtCore/QPair>
#include <QtGui/QOpenGLContext>
#include "rendertimer.h"

#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif

RenderTimer::RenderTimer(QObject* parent) :
    QObject(parent),
    m_type(RenderTimer::Trivial)
{
}

bool RenderTimer::isAvailable(TimerType type)
{
    if (type == RenderTimer::Trivial) {
        return true;

#if defined(QT_OPENGL_ES)
    } else if (type == RenderTimer::KHRFence) {
        QList<QByteArray> eglExtensions = QByteArray(
            static_cast<const char*>(eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
        QList<QByteArray> glExtensions = QByteArray(
            reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
        // Note the workaround for PowerVR that declares 'GL_OES_egl_sync'.
        return eglExtensions.contains("EGL_KHR_fence_sync") &&
                (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"));

    } else if (type == RenderTimer::NVFence) {
        QList<QByteArray> glExtensions = QByteArray(
            reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
        return glExtensions.contains("GL_NV_fence");
#else
    } else if (type == RenderTimer::ARBTimerQuery) {
        // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack of a method to check
        // the presence of glQueryCounter() would force us to inspect OpenGL version and extensions, which
        // is basically as annoying as doing the whole thing here.
        // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
        QOpenGLContext* context = QOpenGLContext::currentContext();
        QSurfaceFormat format = context->format();
        return qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
                && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"));

    } else if (type == RenderTimer::EXTTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        return context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"));
#endif
    }

    return false;
}

RenderTimer::TimerType RenderTimer::optimalTimerType()
{
    QList<TimerType> types;
#if defined(QT_OPENGL_ES)
    types << RenderTimer::KHRFence << RenderTimer::NVFence;
#else
    types << RenderTimer::ARBTimerQuery << RenderTimer::EXTTimerQuery;
#endif

    Q_FOREACH(TimerType type, types) {
        if (RenderTimer::isAvailable(type)) {
            return type;
        }
    }

    return RenderTimer::Trivial;
}

void RenderTimer::setup(TimerType type)
{
    if (type == RenderTimer::Automatic) {
        type = RenderTimer::optimalTimerType();
    }

    m_type = type;

    if (m_type == RenderTimer::Trivial) {
#if defined(QT_OPENGL_ES)
    } else if (m_type == RenderTimer::KHRFence) {
        m_fenceSyncKHR.createSyncKHR = reinterpret_cast<
            EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
            eglGetProcAddress("eglCreateSyncKHR"));
        m_fenceSyncKHR.destroySyncKHR = reinterpret_cast<
            EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
            eglGetProcAddress("eglDestroySyncKHR"));
        m_fenceSyncKHR.clientWaitSyncKHR = reinterpret_cast<
            EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
            eglGetProcAddress("eglClientWaitSyncKHR"));

    } else if (m_type == RenderTimer::NVFence) {
        m_fenceNV.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            eglGetProcAddress("glGenFencesNV"));
        m_fenceNV.deleteFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            eglGetProcAddress("glDeleteFencesNV"));
        m_fenceNV.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            eglGetProcAddress("glSetFenceNV"));
        m_fenceNV.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
            eglGetProcAddress("glFinishFenceNV"));
        m_fenceNV.genFencesNV(2, m_fence);
#else
    } else if (m_type == RenderTimer::ARBTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            context->getProcAddress("glDeleteQueries"));
        m_timerQuery.getQueryObjectui64v =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
            context->getProcAddress("glGetQueryObjectui64v"));
        m_timerQuery.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            context->getProcAddress("glQueryCounter"));
        m_timerQueryVersion = TimerQueryCore;
        m_timerQuery.genQueries(2, m_timer);

    } else if (m_type == RenderTimer::EXTTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            context->getProcAddress("glDeleteQueries"));
        m_timerQuery.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
            context->getProcAddress("glBeginQuery"));
        m_timerQuery.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
            context->getProcAddress("glEndQuery"));
        m_timerQuery.getQueryObjectui64vExt =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
            context->getProcAddress("glGetQueryObjectui64vEXT"));
        m_timerQueryVersion = TimerQueryExt;
        m_timerQuery.genQueries(1, m_timer);
#endif
    }
}

void RenderTimer::teardown()
{
    teardown(m_type);
}

void RenderTimer::teardown(TimerType type)
{
    if (type == RenderTimer::Trivial) {
#if defined(QT_OPENGL_ES)
    } else if (type == RenderTimer::KHRFence) {
        if (m_beforeSync != EGL_NO_SYNC_KHR) {
            m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), m_beforeSync);
        }

    } else if (type == RenderTimer::NVFence) {
        m_fenceNV.deleteFencesNV(2, m_fence);
#else
    } else if (type == RenderTimer::ARBTimerQuery) {
        m_timerQuery.deleteQueries(2, m_timer);

    } else if (type == RenderTimer::EXTTimerQuery) {
        m_timerQuery.deleteQueries(1, m_timer);
#endif
    }
}

void RenderTimer::start()
{
    if (m_type == RenderTimer::Trivial) {
        m_trivialTimer.start();
#if defined(QT_OPENGL_ES)
    } else if (m_type == RenderTimer::KHRFence) {
        m_beforeSync = m_fenceSyncKHR.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);

    } else if (m_type == RenderTimer::NVFence) {
        m_fenceNV.setFenceNV(m_fence[0], GL_ALL_COMPLETED_NV);
#else
    } else if (m_type == RenderTimer::ARBTimerQuery) {
        m_timerQuery.queryCounter(m_timer[0], GL_TIMESTAMP);

    } else if (m_type == RenderTimer::EXTTimerQuery) {
        m_timerQuery.beginQuery(GL_TIME_ELAPSED, m_timer[0]);
#endif
    }
}

qint64 RenderTimer::stop()
{
    if (m_type == RenderTimer::Trivial) {
        glFinish();
        return m_trivialTimer.nsecsElapsed();
#if defined(QT_OPENGL_ES)
    } else if (m_type == RenderTimer::KHRFence) {
        QElapsedTimer timer;
        EGLDisplay dpy = eglGetCurrentDisplay();
        EGLSyncKHR afterSync = m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
        EGLint beforeSyncValue = m_fenceSyncKHR.clientWaitSyncKHR(dpy, m_beforeSync, 0, EGL_FOREVER_KHR);
        qint64 beforeTime = timer.nsecsElapsed();
        EGLint afterSyncValue = m_fenceSyncKHR.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
        qint64 afterTime = timer.nsecsElapsed();
        m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
        m_fenceSyncKHR.destroySyncKHR(dpy, m_beforeSync);
        m_beforeSync = EGL_NO_SYNC_KHR;
        if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
        && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
            return afterTime - beforeTime;
        } else {
            return -1;
        }

    } else if (m_type == RenderTimer::NVFence) {
        QElapsedTimer timer;
        m_fenceNV.setFenceNV(m_fence[1], GL_ALL_COMPLETED_NV);
        m_fenceNV.finishFenceNV(m_fence[0]);
        qint64 beforeTime = timer.nsecsElapsed();
        m_fenceNV.finishFenceNV(m_fence[1]);
        qint64 afterTime = timer.nsecsElapsed();
        return afterTime - beforeTime;
#else
    } else if (m_type == RenderTimer::ARBTimerQuery) {
        GLuint64 time[2] = { 0, 0 };
        m_timerQuery.queryCounter(m_timer[1], GL_TIMESTAMP);
        m_timerQuery.getQueryObjectui64v(m_timer[0], GL_QUERY_RESULT, &time[0]);
        m_timerQuery.getQueryObjectui64v(m_timer[1], GL_QUERY_RESULT, &time[1]);
        if (time[0] != 0 && time[1] != 0) {
            return time[1] - time[0];
        } else {
            return -1;
        }

    } else if (m_type == RenderTimer::EXTTimerQuery) {
        GLuint64EXT time;
        m_timerQuery.endQuery(GL_TIME_ELAPSED);
        m_timerQuery.getQueryObjectui64vExt(m_timer[0], GL_QUERY_RESULT, &time);
        return static_cast<qint64>(time);
#endif
    }

    return 0;
}
