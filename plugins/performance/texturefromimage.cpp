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

#include <QtQuick/QQuickWindow>
#include "texturefromimage.h"

TextureFromImageTextureProvider::TextureFromImageTextureProvider() :
    QSGTextureProvider(),
    m_texture(NULL)
{
}

TextureFromImageTextureProvider::~TextureFromImageTextureProvider()
{
    if (m_texture != NULL) {
        delete m_texture;
    }
}

QSGTexture* TextureFromImageTextureProvider::texture() const
{
    return m_texture;
}

void TextureFromImageTextureProvider::setTexture(QSGTexture* texture)
{
    if (m_texture != NULL) {
        delete m_texture;
    }
    m_texture = texture;

    // FIXME: hardcoded flag
    if (m_texture != NULL) {
        m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    }
}



TextureFromImage::TextureFromImage(QQuickItem* parent) :
    QQuickItem(parent),
    m_textureProvider(NULL),
    m_textureNeedsUpdate(true)
{
    setFlag(QQuickItem::ItemHasContents);
}

TextureFromImage::~TextureFromImage()
{
    if (m_textureProvider != NULL) {
        m_textureProvider->deleteLater();
    }
}

bool TextureFromImage::isTextureProvider() const
{
    return true;
}

QSGTextureProvider* TextureFromImage::textureProvider() const
{
    if (m_textureProvider == NULL) {
        const_cast<TextureFromImage*>(this)->m_textureProvider = new TextureFromImageTextureProvider;
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
    }
    return m_textureProvider;
}

QSGNode* TextureFromImage::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    Q_UNUSED(oldNode)
    Q_UNUSED(updatePaintNodeData)

    if (m_textureNeedsUpdate && m_textureProvider != NULL) {
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
        m_textureNeedsUpdate = false;
    }
    return NULL;
}

QImage TextureFromImage::image() const
{
    return m_image;
}

void TextureFromImage::setImage(QImage image)
{
    if (image != m_image) {
        m_image = image;
        Q_EMIT imageChanged();
        m_textureNeedsUpdate = true;
        update();
    }
}
