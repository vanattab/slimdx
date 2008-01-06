/*
* Copyright (c) 2007 SlimDX Group
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <d3d10.h>
#include <d3dx10.h>

#include "GraphicsException.h"

#include "Sprite.h"
#include "Device.h"

namespace SlimDX
{
namespace Direct3D10
{
	Sprite::Sprite( IntPtr pointer )
	{
		if( pointer == IntPtr::Zero )
			throw gcnew ArgumentNullException( "pointer" );

		void* result;
		IUnknown* unknown = static_cast<IUnknown*>( pointer.ToPointer() );
		HRESULT hr = unknown->QueryInterface( IID_ID3DX10Sprite, &result );
		if( FAILED( hr ) )
			throw gcnew InvalidCastException( "Failed to QueryInterface on user-supplied pointer." );

		m_Pointer = static_cast<ID3DX10Sprite*>( result );
	}

	Sprite::Sprite( ID3DX10Sprite* sprite )
	{
		if( sprite == NULL )
			throw gcnew ArgumentNullException( "sprite" );

		m_Pointer = sprite;
	}

	Sprite::Sprite( Device^ device, int bufferSize )
	{
		ID3DX10Sprite* sprite;
		
		HRESULT hr = D3DX10CreateSprite( device->DevicePointer, bufferSize, &sprite );
		GraphicsException::CheckHResult( hr );

		m_Pointer = sprite;
	}
	
	Matrix Sprite::ViewTransform::get()
	{
		D3DXMATRIX matrix;
		HRESULT hr = m_Pointer->GetViewTransform( &matrix );
		GraphicsException::CheckHResult( hr );
		return Matrix::FromD3DXMATRIX( matrix );
	}

	void Sprite::ViewTransform::set( Matrix value )
	{
		HRESULT hr = m_Pointer->SetViewTransform( reinterpret_cast<D3DXMATRIX*>( &value ) );
		GraphicsException::CheckHResult( hr );
	}

	Matrix Sprite::ProjectionTransform::get()
	{
		D3DXMATRIX matrix;
		HRESULT hr = m_Pointer->GetProjectionTransform( &matrix );
		GraphicsException::CheckHResult( hr );
		return Matrix::FromD3DXMATRIX( matrix );
	}

	void Sprite::ProjectionTransform::set( Matrix value )
	{
		HRESULT hr = m_Pointer->SetProjectionTransform( reinterpret_cast<D3DXMATRIX*>( &value ) );
		GraphicsException::CheckHResult( hr );
	}

	void Sprite::Begin( SpriteFlags flags )
	{
		HRESULT hr = m_Pointer->Begin( static_cast<DWORD>( flags ) );
		GraphicsException::CheckHResult( hr );
	}

	void Sprite::End()
	{
		HRESULT hr = m_Pointer->End();
		GraphicsException::CheckHResult( hr );
	}

	void Sprite::Flush()
	{
		HRESULT hr = m_Pointer->Flush();
		GraphicsException::CheckHResult( hr );
	}

	void Sprite::DrawBuffered( array<SpriteInstance^>^ instances )
	{
		D3DX10_SPRITE* nativeInstances = new D3DX10_SPRITE[instances->Length];
		for( int instanceIndex = 0; instanceIndex < instances->Length; ++instanceIndex )
			instances[instanceIndex]->ToNativeObject( nativeInstances[instanceIndex] );
		HRESULT hr = m_Pointer->DrawSpritesBuffered( nativeInstances, instances->Length );
		GraphicsException::CheckHResult( hr );
	}

	void Sprite::DrawImmediate( array<SpriteInstance^>^ instances )
	{
		D3DX10_SPRITE* nativeInstances = new D3DX10_SPRITE[instances->Length];
		for( int instanceIndex = 0; instanceIndex < instances->Length; ++instanceIndex )
			instances[instanceIndex]->ToNativeObject( nativeInstances[instanceIndex] );
		HRESULT hr = m_Pointer->DrawSpritesImmediate( nativeInstances, instances->Length, sizeof(D3DX10_SPRITE), 0 );
		GraphicsException::CheckHResult( hr );
	}
}
}
