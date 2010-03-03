#include "stdafx.h"
/*
* Copyright (c) 2007-2010 SlimDX Group
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

#include "IDWriteFontCollectionMock.h"
#include "IDWriteTextLayoutMock.h"

using namespace testing;
using namespace System;
using namespace SlimDX;
using namespace SlimDX::DirectWrite;

ref class MockedTextLayout
{
public:
	MockedTextLayout()
		: mockLayout(new IDWriteTextLayoutMock),
		layout(TextLayout::FromPointer(System::IntPtr(mockLayout)))
	{
	}
	~MockedTextLayout()
	{
		delete layout;
		layout = nullptr;
		delete mockLayout;
		mockLayout = 0;
	}
	property IDWriteTextLayoutMock &Mock
	{
		IDWriteTextLayoutMock &get() { return *mockLayout; }
	}
	property TextLayout ^Layout
	{
		TextLayout ^get() { return layout; }
	}

private:
	IDWriteTextLayoutMock *mockLayout;
	TextLayout ^layout;
};

class TextLayoutTest : public testing::Test
{
protected:
	virtual void TearDown()
	{
		ASSERT_EQ(0, ObjectTable::Objects->Count);
	}
};

static DWRITE_CLUSTER_METRICS ExpectedClusterMetrics()
{
	DWRITE_CLUSTER_METRICS expectedMetrics;
	expectedMetrics.width = 15.0f;
	expectedMetrics.length = 1;
	expectedMetrics.canWrapLineAfter = 0;
	expectedMetrics.isWhitespace = 0;
	expectedMetrics.isNewline = 0;
	expectedMetrics.isSoftHyphen = 0;
	expectedMetrics.isRightToLeft = 0;
	return expectedMetrics;
}

static void AssertClusterMetricsMatchExpected(ClusterMetrics metrics)
{
	DWRITE_CLUSTER_METRICS expected = ExpectedClusterMetrics();
	ASSERT_EQ( expected.width, metrics.Width );
	ASSERT_EQ( expected.length, metrics.Length );
	ASSERT_EQ( expected.canWrapLineAfter != 0, metrics.CanWrapLineAfter );
	ASSERT_EQ( expected.isWhitespace != 0, metrics.IsWhitespace );
	ASSERT_EQ( expected.isNewline != 0, metrics.IsNewline );
	ASSERT_EQ( expected.isSoftHyphen != 0, metrics.IsSoftHyphen );
	ASSERT_EQ( expected.isRightToLeft != 0, metrics.IsRightToLeft );
}

TEST_F(TextLayoutTest, GetClusterMetrics)
{
	MockedTextLayout layout;
	EXPECT_CALL(layout.Mock, GetClusterMetrics(_, _, _))
		.WillRepeatedly(Return(E_UNEXPECTED));
	EXPECT_CALL(layout.Mock, GetClusterMetrics(0, 0, NotNull()))
		.WillOnce(DoAll(SetArgumentPointee<2>(1),
						Return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))));
	EXPECT_CALL(layout.Mock, GetClusterMetrics(NotNull(), Gt(0U), NotNull()))
		.WillOnce(DoAll(SetArgumentPointee<0>(ExpectedClusterMetrics()),
						SetArgumentPointee<2>(1),
						Return(S_OK)));
	array<SlimDX::DirectWrite::ClusterMetrics>^ metrics = layout.Layout->GetClusterMetrics();
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_TRUE( metrics != nullptr );
	ASSERT_EQ( 1, metrics->Length );
	AssertClusterMetricsMatchExpected(metrics[0]);
}

TEST_F(TextLayoutTest, DetermineMinWidth)
{
	MockedTextLayout layout;
	EXPECT_CALL(layout.Mock, DetermineMinWidth(NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<0>(14.5f), Return(S_OK)));
	float minWidth = -1.0f;
	ASSERT_EQ( 14.5f, layout.Layout->DetermineMinWidth() );
}

static DWRITE_HIT_TEST_METRICS ExpectedHitTestMetrics()
{
	DWRITE_HIT_TEST_METRICS expectedMetrics;
	expectedMetrics.textPosition = 0;
	expectedMetrics.length = 1;
	expectedMetrics.left = 0.15f;
	expectedMetrics.top = 0.245f;
	expectedMetrics.width = 45.8f;
	expectedMetrics.height = 998.5f;
	expectedMetrics.bidiLevel = 23;
	expectedMetrics.isText = TRUE;
	expectedMetrics.isTrimmed = TRUE;
	return expectedMetrics;
}

static void AssertHitTestMetricsMatchExpected(HitTestMetrics% actual)
{
	DWRITE_HIT_TEST_METRICS expected = ExpectedHitTestMetrics();
	ASSERT_EQ( expected.textPosition, actual.TextPosition );
	ASSERT_EQ( expected.length, actual.Length );
	ASSERT_EQ( expected.left, actual.Left );
	ASSERT_EQ( expected.top, actual.Top );
	ASSERT_EQ( expected.width, actual.Width );
	ASSERT_EQ( expected.height, actual.Height );
	ASSERT_EQ( expected.bidiLevel, actual.BidiLevel );
	ASSERT_EQ( expected.isText == TRUE, actual.IsText );
	ASSERT_EQ( expected.isTrimmed == TRUE, actual.IsTrimmed );
}

TEST_F(TextLayoutTest, HitTestPoint)
{
	MockedTextLayout layout;
	EXPECT_CALL(layout.Mock, HitTestPoint(10.0f, 12.0f, NotNull(), NotNull(), NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<2>(false),
						SetArgumentPointee<3>(false),
						SetArgumentPointee<4>(ExpectedHitTestMetrics()),
						Return(S_OK)));
	bool isTrailingHit = true;
	bool isInside = true;
	HitTestMetrics metrics = layout.Layout->HitTestPoint(10.0f, 12.0f, isTrailingHit, isInside);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_FALSE( isTrailingHit );
	ASSERT_FALSE( isInside );
	AssertHitTestMetricsMatchExpected(metrics);
}

TEST_F(TextLayoutTest, HitTestTextPosition)
{
	MockedTextLayout layout;
	EXPECT_CALL(layout.Mock, HitTestTextPosition(12U, FALSE, NotNull(), NotNull(), NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<2>(12.5f),
						SetArgumentPointee<3>(13.5f),
						SetArgumentPointee<4>(ExpectedHitTestMetrics()),
						Return(S_OK)));
	float x, y;
	HitTestMetrics metrics = layout.Layout->HitTestTextPosition(12U, false, x, y);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_EQ( 12.5f, x );
	ASSERT_EQ( 13.5f, y );
	AssertHitTestMetricsMatchExpected(metrics);
}

TEST_F(TextLayoutTest, HitTestTextRange)
{
	MockedTextLayout layout;
	EXPECT_CALL(layout.Mock, HitTestTextRange(12U, 4U, 10.0f, 12.0f, 0, 0, NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<6>(1U), Return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))));
	EXPECT_CALL(layout.Mock, HitTestTextRange(12U, 4U, 10.0f, 12.0f, NotNull(), 1U, NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<6>(1U),
						SetArgumentPointee<4>(ExpectedHitTestMetrics()),
						Return(S_OK)));
	array<HitTestMetrics> ^metrics = layout.Layout->HitTestTextRange(12U, 4U, 10.0f, 12.0f);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_EQ( 1, metrics->Length );
	AssertHitTestMetricsMatchExpected(metrics[0]);
}

TEST_F(TextLayoutTest, GetFontCollectionNoTextRange)
{
	MockedTextLayout layout;
	IDWriteFontCollectionMock mockCollection;
	EXPECT_CALL(layout.Mock, GetFontCollection(0, NotNull(), 0))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<1>(&mockCollection), Return(S_OK)));
	FontCollection ^collection = layout.Layout->GetFontCollection(0);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_TRUE( collection != nullptr );
	ASSERT_EQ( collection->InternalPointer, &mockCollection );
	delete collection;
}

TEST_F(TextLayoutTest, GetFontCollectionWithTextRange)
{
	MockedTextLayout layout;
	IDWriteFontCollectionMock mockCollection;
	DWRITE_TEXT_RANGE fakeTextRange;
	fakeTextRange.length = 1;
	fakeTextRange.startPosition = 0;
	EXPECT_CALL(layout.Mock, GetFontCollection(0, NotNull(), NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<1>(&mockCollection),
						SetArgumentPointee<2>(fakeTextRange),
						Return(S_OK)));
	TextRange textRange;
	FontCollection ^collection = layout.Layout->GetFontCollection(0, textRange);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_TRUE( collection != nullptr );
	ASSERT_EQ( collection->InternalPointer, &mockCollection );
	ASSERT_EQ( 1, textRange.Length );
	ASSERT_EQ( 0, textRange.StartPosition );
	delete collection;
}

#define NUM_OF(ary_) (sizeof(ary_)/sizeof(ary_[0]))

TEST_F(TextLayoutTest, GetFontFamilyNameNoTextRange)
{
	MockedTextLayout layout;
	WCHAR fakeName[] = L"Slartibartfast";
	int const numFakeName = NUM_OF(fakeName);
	EXPECT_CALL(layout.Mock, GetFontFamilyNameLength(0, NotNull(), 0))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<1>(numFakeName), Return(S_OK)));
	EXPECT_CALL(layout.Mock, GetFontFamilyName(0, NotNull(), numFakeName, 0))
		.Times(1)
		.WillOnce(DoAll(SetArrayArgument<1>(&fakeName[0], &fakeName[numFakeName]),
						Return(S_OK)));
	String^ name = layout.Layout->GetFontFamilyName(0);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_TRUE( name != nullptr );
	ASSERT_TRUE( gcnew String(fakeName) == name );
}

TEST_F(TextLayoutTest, GetFontFamilyNameWithTextRange)
{
	MockedTextLayout layout;
	WCHAR fakeName[] = L"Slartibartfast";
	int const numFakeName = NUM_OF(fakeName);
	EXPECT_CALL(layout.Mock, GetFontFamilyNameLength(0, NotNull(), NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArgumentPointee<1>(numFakeName), Return(S_OK)));
	DWRITE_TEXT_RANGE fakeTextRange;
	fakeTextRange.startPosition = 1;
	fakeTextRange.length = 2;
	EXPECT_CALL(layout.Mock, GetFontFamilyName(0, NotNull(), numFakeName, NotNull()))
		.Times(1)
		.WillOnce(DoAll(SetArrayArgument<1>(&fakeName[0], &fakeName[numFakeName]),
						SetArgumentPointee<3>(fakeTextRange),
						Return(S_OK)));
	TextRange range;
	String^ name = layout.Layout->GetFontFamilyName(0, range);
	ASSERT_TRUE( Result::Last.IsSuccess );
	ASSERT_TRUE( name != nullptr );
	ASSERT_TRUE( gcnew String("Slartibartfast") == name );
	ASSERT_EQ(1, range.StartPosition );
	ASSERT_EQ(2, range.Length );
}

static bool operator==(DWRITE_TEXT_RANGE const &lhs, DWRITE_TEXT_RANGE const &rhs)
{
	return lhs.startPosition == rhs.startPosition
		&& lhs.length == rhs.length;
}

TEST_F(TextLayoutTest, SetFontFamilyName)
{
	MockedTextLayout layout;
	DWRITE_TEXT_RANGE expectedRange;
	expectedRange.startPosition = 2;
	expectedRange.length = 4;
	EXPECT_CALL(layout.Mock, SetFontFamilyName(NotNull(), expectedRange))
		.Times(1)
		.WillOnce(Return(S_OK));
	
	ASSERT_TRUE( layout.Layout->SetFontFamilyName(gcnew String("Slartibartfast!"), TextRange(2, 4)).IsSuccess );
}