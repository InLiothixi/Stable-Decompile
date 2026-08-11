#include <time.h>
#include "LawnApp.h"
#include "Lawn/Board.h"
#include "Lawn/Plant.h"
#include "Lawn/Zombie.h"
#include "Lawn/Cutscene.h"
#include "GameConstants.h"
#include "Lawn/Challenge.h"
#include "Lawn/ZenGarden.h"
#include "Sexy.TodLib/Trail.h"
#include "Lawn/System/Music.h"
#include "Lawn/System/SaveGame.h"
#include "Sexy.TodLib/TodDebug.h"
#include "Sexy.TodLib/TodFoley.h"
#include "Sexy.TodLib/Attachment.h"
#include "Lawn/System/PlayerInfo.h"
#include "Lawn/System/PoolEffect.h"
#include "Lawn/System/ProfileMgr.h"
#include "Lawn/System/PopDRMComm.h"
#include "Lawn/Widget/GameButton.h"
#include "Sexy.TodLib/Reanimator.h"
#include "Lawn/Widget/UserDialog.h"
#include "Lawn/System/TypingCheck.h"
#include "Sexy.TodLib/TodParticle.h"
#include "Lawn/Widget/AwardScreen.h"
#include "Lawn/Widget/TitleScreen.h"
#include "Lawn/Widget/StoreScreen.h"
#include "Lawn/Widget/CheatDialog.h"
#include "Lawn/Widget/GameSelector.h"
#include "Lawn/Widget/CreditScreen.h"
#include "Sexy.TodLib/EffectSystem.h"
#include "Sexy.TodLib/FilterEffect.h"
#include "SexyAppFramework/Graphics.h"
#include "Sexy.TodLib/TodStringFile.h"
#include "Lawn/Widget/AlmanacDialog.h"
#include "Lawn/Widget/NewUserDialog.h"
#include "Lawn/Widget/ContinueDialog.h"
#include "Lawn/Widget/ZombatarTOS.h"
#include "Lawn/Widget/LanguageScreen.h"
#include "Lawn/System/ReanimationLawn.h"
#include "Lawn/Widget/ChallengeScreen.h"
#include "Lawn/Widget/NewOptionsDialog.h"
#include "Lawn/Widget/SeedChooserScreen.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ResourceManager.h"

#include "SexyAppFramework/Checkbox.h"
#include "SexyAppFramework/BassMusicInterface.h"
#include "SexyAppFramework/Dialog.h"
#include "SexyAppFramework/resource.h"

#include "Lawn/SeedPacket.h"

#include "Sexy.TodLib/Definition.h"

#include "portaudio.h"

#include "Editor/ParticleScreen.h"
#include "SexyAppFramework/D3DInterface.h"

#include "Lawn/Widget/MoreSettingsDialog.h"

#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

bool gIsPartnerBuild = false;
bool gSlowMo = false;  //0x6A9EAA
bool gFastMo = false;  //0x6A9EAB
LawnApp* gLawnApp = nullptr;  //0x6A9EC0
int gSlowMoCounter = 0;  //0x6A9EC4

#include "SexyAppFramework/Debug.h"

#include <SDL3/SDL.h>
#include "SexyAppFramework/SDL3Image.h"
SDL_Window* LawnApp::mSDLWindow = nullptr;
SDL_Renderer* LawnApp::mSDLRenderer = nullptr;
SDL_Cursor* LawnApp::mSDLPointerCursor = nullptr;
SDL_Cursor* LawnApp::mSDLHandCursor = nullptr;
SDL_Cursor* LawnApp::mSDLDraggingCursor = nullptr;
SDL_Cursor* LawnApp::mSDLTextCursor = nullptr;
SDL_Cursor* LawnApp::mSDLWaitCursor = nullptr;
SDL_Cursor* LawnApp::mSDLNoCursor = nullptr;

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <SDL3_ttf/SDL_ttf.h>


bool LawnApp::mIsPlayingVideo = false;

namespace
{
	int GetVideoSwsColorspace(const AVFrame* theFrame, const AVCodecContext* theDecoder)
	{
		const AVColorSpace aColorspace = theFrame->colorspace != AVCOL_SPC_UNSPECIFIED
			? theFrame->colorspace
			: theDecoder->colorspace;
		switch (aColorspace)
		{
		case AVCOL_SPC_BT709:
			return SWS_CS_ITU709;
		case AVCOL_SPC_FCC:
			return SWS_CS_FCC;
		case AVCOL_SPC_BT470BG:
		case AVCOL_SPC_SMPTE170M:
			return SWS_CS_ITU601;
		case AVCOL_SPC_SMPTE240M:
			return SWS_CS_SMPTE240M;
		case AVCOL_SPC_BT2020_NCL:
		case AVCOL_SPC_BT2020_CL:
			return SWS_CS_BT2020;
		default:
			// Unspecified HD video is conventionally BT.709; SD video is BT.601.
			return theFrame->width >= 720 || theFrame->height >= 576
				? SWS_CS_ITU709
				: SWS_CS_ITU601;
		}
	}

	bool IsVideoFullRange(const AVFrame* theFrame, const AVCodecContext* theDecoder)
	{
		const AVColorRange aRange = theFrame->color_range != AVCOL_RANGE_UNSPECIFIED
			? theFrame->color_range
			: theDecoder->color_range;
		return aRange == AVCOL_RANGE_JPEG;
	}

	SDL_Texture* CreateVideoTexture(
		SDL_Renderer* theRenderer,
		SDL_PixelFormat theFormat,
		int theWidth,
		int theHeight,
		SDL_Colorspace theColorspace)
	{
		SDL_PropertiesID aProperties = SDL_CreateProperties();
		if (aProperties == 0)
			return nullptr;

		SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, theFormat);
		SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STREAMING);
		SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, theWidth);
		SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, theHeight);
		SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, theColorspace);

		SDL_Texture* aTexture = SDL_CreateTextureWithProperties(theRenderer, aProperties);
		SDL_DestroyProperties(aProperties);
		return aTexture;
	}

	SDL_Renderer* CreateLawnRenderer(SDL_Window* theWindow, bool enableNativeHDR, bool enableVsync)
	{
		SDL_PropertiesID aProperties = SDL_CreateProperties();
		if (aProperties == 0)
			return nullptr;

		SDL_SetPointerProperty(aProperties, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, theWindow);
		if (enableNativeHDR)
		{
			SDL_SetStringProperty(
				aProperties,
				SDL_PROP_RENDERER_CREATE_NAME_STRING,
				"direct3d11"
			);
		}
		SDL_SetNumberProperty(
			aProperties,
			SDL_PROP_RENDERER_CREATE_OUTPUT_COLORSPACE_NUMBER,
			enableNativeHDR ? SDL_COLORSPACE_SRGB_LINEAR : SDL_COLORSPACE_SRGB
		);
		SDL_SetNumberProperty(aProperties, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, enableVsync ? 1 : 0);

		SDL_Renderer* aRenderer = SDL_CreateRendererWithProperties(aProperties);
		SDL_DestroyProperties(aProperties);

		if (aRenderer != nullptr && enableNativeHDR)
		{
			SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(aRenderer);
			SDL_Colorspace anOutputColorspace = (SDL_Colorspace)SDL_GetNumberProperty(
				aRendererProperties,
				SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
				SDL_COLORSPACE_UNKNOWN
			);
			if (anOutputColorspace != SDL_COLORSPACE_SRGB_LINEAR)
			{
				SDL_DestroyRenderer(aRenderer);
				SDL_SetError("The selected renderer did not create an scRGB output");
				return nullptr;
			}
		}

		return aRenderer;
	}

	bool ShouldUseSDRVideoPresentation(SDL_Renderer* theRenderer)
	{
		if (theRenderer == nullptr)
			return false;

		const SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(theRenderer);
		const SDL_Colorspace anOutputColorspace = (SDL_Colorspace)SDL_GetNumberProperty(
			aRendererProperties,
			SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
			SDL_COLORSPACE_UNKNOWN
		);
		return anOutputColorspace == SDL_COLORSPACE_SRGB_LINEAR &&
			SDL_GetBooleanProperty(aRendererProperties, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false);
	}

	bool SDLCALL KeepEventsForOtherWindows(void* theUserData, SDL_Event* theEvent)
	{
		SDL_Window* aWindow = static_cast<SDL_Window*>(theUserData);
		return SDL_GetWindowFromEvent(theEvent) != aWindow;
	}

	struct SDRVideoPresentation
	{
		SDL_Window* mWindow = nullptr;
		SDL_Renderer* mRenderer = nullptr;
		SDL_WindowID mWindowId = 0;

		~SDRVideoPresentation()
		{
			Destroy();
		}

		SDRVideoPresentation(const SDRVideoPresentation&) = delete;
		SDRVideoPresentation& operator=(const SDRVideoPresentation&) = delete;
		SDRVideoPresentation() = default;

		bool Create(
			SDL_Window* theParent,
			int theLogicalWidth,
			int theLogicalHeight,
			Uint8 theRed,
			Uint8 theGreen,
			Uint8 theBlue,
			Uint8 theAlpha)
		{
			int aParentWidth = 0;
			int aParentHeight = 0;
			if (theParent == nullptr ||
				!SDL_GetWindowSize(theParent, &aParentWidth, &aParentHeight) ||
				aParentWidth <= 0 || aParentHeight <= 0)
			{
				TodTrace("Video SDR popup could not query the parent window size: %s\n", SDL_GetError());
				return false;
			}

			SDL_PropertiesID aWindowProperties = SDL_CreateProperties();
			if (aWindowProperties == 0)
			{
				TodTrace("Video SDR popup property creation failed: %s\n", SDL_GetError());
				return false;
			}

			const bool aWindowPropertiesConfigured =
				SDL_SetPointerProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, theParent) &&
				SDL_SetBooleanProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_TOOLTIP_BOOLEAN, true) &&
				SDL_SetBooleanProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, false) &&
				SDL_SetBooleanProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_CONSTRAIN_POPUP_BOOLEAN, false) &&
				SDL_SetBooleanProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true) &&
				SDL_SetBooleanProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true) &&
				SDL_SetNumberProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, 0) &&
				SDL_SetNumberProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 0) &&
				SDL_SetNumberProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, aParentWidth) &&
				SDL_SetNumberProperty(aWindowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, aParentHeight);
			if (aWindowPropertiesConfigured)
				mWindow = SDL_CreateWindowWithProperties(aWindowProperties);
			SDL_DestroyProperties(aWindowProperties);

			if (!aWindowPropertiesConfigured || mWindow == nullptr)
			{
				TodTrace("Video SDR popup creation failed: %s\n", SDL_GetError());
				Destroy();
				return false;
			}

			mWindowId = SDL_GetWindowID(mWindow);
			SDL_PropertiesID aRendererProperties = SDL_CreateProperties();
			if (aRendererProperties == 0)
			{
				TodTrace("Video SDR renderer property creation failed: %s\n", SDL_GetError());
				Destroy();
				return false;
			}

			const bool aRendererPropertiesConfigured =
				SDL_SetPointerProperty(aRendererProperties, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, mWindow) &&
				SDL_SetStringProperty(aRendererProperties, SDL_PROP_RENDERER_CREATE_NAME_STRING, "direct3d11") &&
				SDL_SetNumberProperty(
					aRendererProperties,
					SDL_PROP_RENDERER_CREATE_OUTPUT_COLORSPACE_NUMBER,
					SDL_COLORSPACE_SRGB
				) &&
				SDL_SetNumberProperty(aRendererProperties, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 1);
			if (aRendererPropertiesConfigured)
				mRenderer = SDL_CreateRendererWithProperties(aRendererProperties);
			SDL_DestroyProperties(aRendererProperties);

			if (!aRendererPropertiesConfigured || mRenderer == nullptr)
			{
				TodTrace("Video SDR renderer creation failed: %s\n", SDL_GetError());
				Destroy();
				return false;
			}

			const SDL_PropertiesID anActualRendererProperties = SDL_GetRendererProperties(mRenderer);
			const SDL_Colorspace anActualColorspace = (SDL_Colorspace)SDL_GetNumberProperty(
				anActualRendererProperties,
				SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
				SDL_COLORSPACE_UNKNOWN
			);
			if (anActualColorspace != SDL_COLORSPACE_SRGB)
			{
				TodTrace(
					"Video SDR renderer returned output colorspace 0x%X instead of sRGB.\n",
					(unsigned int)anActualColorspace
				);
				Destroy();
				return false;
			}

			if (!SDL_SetRenderLogicalPresentation(
				mRenderer,
				theLogicalWidth,
				theLogicalHeight,
				SDL_LOGICAL_PRESENTATION_LETTERBOX
			) ||
				!SDL_SetRenderDrawColor(mRenderer, theRed, theGreen, theBlue, theAlpha) ||
				!SDL_RenderClear(mRenderer) ||
				!SDL_RenderPresent(mRenderer) ||
				!SDL_ShowWindow(mWindow) ||
				!SDL_SyncWindow(mWindow))
			{
				TodTrace("Video SDR popup initialization failed: %s\n", SDL_GetError());
				Destroy();
				return false;
			}

			TodTrace(
				"Video SDR compatibility renderer: %s, output colorspace: 0x%X, popup: %dx%d.\n",
				SDL_GetRendererName(mRenderer),
				(unsigned int)anActualColorspace,
				aParentWidth,
				aParentHeight
			);
			return true;
		}

		void Destroy()
		{
			if (mWindow != nullptr)
				SDL_HideWindow(mWindow);
			if (mRenderer != nullptr)
			{
				SDL_DestroyRenderer(mRenderer);
				mRenderer = nullptr;
			}
			if (mWindow != nullptr)
			{
				// Remove auxiliary focus/window/render events while SDL can still
				// resolve their window ID. The normal game loop must only see events
				// that belong to the persistent main window.
				SDL_PumpEvents();
				SDL_FilterEvents(KeepEventsForOtherWindows, mWindow);
				SDL_DestroyWindow(mWindow);
				mWindow = nullptr;
			}
			mWindowId = 0;
		}
	};

}

// I wouldn't be able to make this without Codotaku. Huge W for them
bool LawnApp::PlayVideo(std::string url, bool isSkipable, Color bgColor)
{
	mIsPlayingVideo = true;

	AVFormatContext* format_context = NULL;
	const int ret = avformat_open_input(&format_context, url.c_str(), NULL, NULL);
	if (ret < 0) 
	{
		TodTrace("Video: %s is missing or corrupted!\n", url.c_str());
		mIsPlayingVideo = false;
		return false;
	}

	SDL_HideCursor();

	const AVCodec* video_codec = NULL;
	const int video_stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, &video_codec, 0);
	const AVStream* video_stream = format_context->streams[video_stream_index];

	const AVCodec* audio_codec = NULL;
	const int  audio_stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, video_stream_index, &audio_codec, 0);
	const AVStream* audio_stream = format_context->streams[audio_stream_index];

	AVCodecContext* video_decoder = avcodec_alloc_context3(video_codec);
	video_decoder->thread_count = 0;
	avcodec_parameters_to_context(video_decoder, video_stream->codecpar);
	avcodec_open2(video_decoder, video_codec, NULL);

	AVCodecContext* audio_decoder = avcodec_alloc_context3(audio_codec);
	audio_decoder->thread_count = 0;
	avcodec_parameters_to_context(audio_decoder, audio_stream->codecpar);
	avcodec_open2(audio_decoder, audio_codec, NULL);

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	SDL_AudioSpec audio_spec = { SDL_AUDIO_F32, audio_decoder->ch_layout.nb_channels, audio_decoder->sample_rate };
	SDL_AudioStream* audio_playback_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
	if (audio_playback_stream == nullptr)
		TodTrace("Video audio device creation failed: %s. Continuing without audio.\n", SDL_GetError());

	SDRVideoPresentation anSDRPresentation;
	SDL_Renderer* aVideoRenderer = mSDLRenderer;
	if (ShouldUseSDRVideoPresentation(mSDLRenderer))
	{
		if (anSDRPresentation.Create(
			mSDLWindow,
			mWidth,
			mHeight,
			bgColor.mRed,
			bgColor.mGreen,
			bgColor.mBlue,
			bgColor.mAlpha
		))
		{
			aVideoRenderer = anSDRPresentation.mRenderer;
		}
		else
		{
			TodTrace("Video SDR compatibility presentation is unavailable; using the main renderer.\n");
		}
	}
	const bool isUsingSDRPresentation = aVideoRenderer == anSDRPresentation.mRenderer &&
		anSDRPresentation.mRenderer != nullptr;
	// MakeWindow and popup synchronization can leave startup geometry events in
	// the queue. Only a change made after the SDR surface is ready should end it.
	const Uint64 aVideoPresentationReadyTimestamp = SDL_GetTicksNS();
	const SDL_WindowID aMainWindowId = SDL_GetWindowID(mSDLWindow);
	const SDL_WindowID aVideoWindowId = isUsingSDRPresentation
		? anSDRPresentation.mWindowId
		: aMainWindowId;

	SDL_Texture* texture = CreateVideoTexture(
		aVideoRenderer,
		SDL_PIXELFORMAT_BGRA32,
		video_decoder->width,
		video_decoder->height,
		SDL_COLORSPACE_SRGB
	);
	if (texture == nullptr)
	{
		TodTrace("Video texture creation failed: %s\n", SDL_GetError());
		SDL_DestroyAudioStream(audio_playback_stream);
		av_frame_free(&frame);
		av_packet_free(&packet);
		avcodec_free_context(&audio_decoder);
		avcodec_free_context(&video_decoder);
		avformat_close_input(&format_context);
		mIsPlayingVideo = false;
		SDL_ShowCursor();
		return false;
	}

	if (!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE))
		TodTrace("Video texture blend-mode setup failed: %s\n", SDL_GetError());
	if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR))
		TodTrace("Video texture scale-mode setup failed: %s\n", SDL_GetError());

	Uint8* bgraPlanes[4] = { nullptr, nullptr, nullptr, nullptr };
	int bgraStrides[4] = { 0, 0, 0, 0 };
	const int aBgraBufferSize = av_image_get_buffer_size(
		AV_PIX_FMT_BGRA,
		video_decoder->width,
		video_decoder->height,
		1
	);
	Uint8* bgraPixels = aBgraBufferSize > 0
		? (Uint8*)av_malloc((size_t)aBgraBufferSize)
		: nullptr;
	if (bgraPixels == nullptr || av_image_fill_arrays(
		bgraPlanes,
		bgraStrides,
		bgraPixels,
		AV_PIX_FMT_BGRA,
		video_decoder->width,
		video_decoder->height,
		1
	) < 0)
	{
		TodTrace("Unable to allocate the video conversion buffer.\n");
		av_free(bgraPixels);
		SDL_DestroyTexture(texture);
		SDL_DestroyAudioStream(audio_playback_stream);
		av_frame_free(&frame);
		av_packet_free(&packet);
		avcodec_free_context(&audio_decoder);
		avcodec_free_context(&video_decoder);
		avformat_close_input(&format_context);
		mIsPlayingVideo = false;
		SDL_ShowCursor();
		return false;
	}

	int previousVsync = mEnableVsync ? 1 : 0;
	const bool havePreviousVsync = !isUsingSDRPresentation &&
		SDL_GetRenderVSync(aVideoRenderer, &previousVsync);
	if (!SDL_SetRenderVSync(aVideoRenderer, 1))
		TodTrace("Video VSync setup failed: %s\n", SDL_GetError());

	bool willShutdown = false;
	bool videoFailed = false;
	bool toggleFullscreenAfterVideo = false;
	Uint64 start_ns = 0;
	const double timelineStartSeconds = format_context->start_time == AV_NOPTS_VALUE
		? 0.0
		: (double)format_context->start_time / AV_TIME_BASE;
	SwsContext* videoScaler = nullptr;
	std::vector<AVPacket*> videoPackets;
	if (video_stream->nb_frames > 0)
		videoPackets.reserve((size_t)video_stream->nb_frames);

	auto presentVideoFrame = [&](AVFrame* theFrame) -> bool
	{
		int64_t aTimestamp = theFrame->best_effort_timestamp;
		if (aTimestamp == AV_NOPTS_VALUE)
			aTimestamp = theFrame->pts;
		if (aTimestamp != AV_NOPTS_VALUE)
		{
			double aFrameTimeSeconds = (double)aTimestamp * av_q2d(video_stream->time_base) - timelineStartSeconds;
			if (aFrameTimeSeconds < 0.0)
				aFrameTimeSeconds = 0.0;
			const double anElapsedSeconds = (double)(SDL_GetTicksNS() - start_ns) / SDL_NS_PER_SECOND;
			const double aDelaySeconds = aFrameTimeSeconds - anElapsedSeconds;
			if (aDelaySeconds > 0.0)
				SDL_DelayPrecise((Uint64)(aDelaySeconds * SDL_NS_PER_SECOND));
			else if (aDelaySeconds < -0.5)
				return true;
		}

		if (theFrame->format < 0 ||
			theFrame->width != video_decoder->width ||
			theFrame->height != video_decoder->height)
		{
			TodTrace(
				"Unsupported decoded video frame (format %d, %dx%d).\n",
				theFrame->format,
				theFrame->width,
				theFrame->height
			);
			return false;
		}

		videoScaler = sws_getCachedContext(
			videoScaler,
			theFrame->width,
			theFrame->height,
			(AVPixelFormat)theFrame->format,
			video_decoder->width,
			video_decoder->height,
			AV_PIX_FMT_BGRA,
			SWS_BILINEAR,
			nullptr,
			nullptr,
			nullptr
		);
		if (videoScaler == nullptr)
		{
			TodTrace("Unable to create the video pixel converter.\n");
			return false;
		}

		const int* aColorCoefficients = sws_getCoefficients(GetVideoSwsColorspace(theFrame, video_decoder));
		if (aColorCoefficients == nullptr || sws_setColorspaceDetails(
			videoScaler,
			aColorCoefficients,
			IsVideoFullRange(theFrame, video_decoder) ? 1 : 0,
			aColorCoefficients,
			1,
			0,
			1 << 16,
			1 << 16
		) < 0)
		{
			TodTrace("Unable to configure the video color conversion.\n");
			return false;
		}

		const Uint8* aSourcePlanes[4] = {
			theFrame->data[0],
			theFrame->data[1],
			theFrame->data[2],
			theFrame->data[3]
		};
		const int aConvertedRows = sws_scale(
			videoScaler,
			aSourcePlanes,
			theFrame->linesize,
			0,
			theFrame->height,
			bgraPlanes,
			bgraStrides
		);
		if (aConvertedRows != video_decoder->height)
		{
			TodTrace("Video pixel conversion failed after %d rows.\n", aConvertedRows);
			return false;
		}

		if (!SDL_UpdateTexture(texture, nullptr, bgraPixels, bgraStrides[0]))
		{
			TodTrace("Video texture upload failed: %s\n", SDL_GetError());
			return false;
		}

		const float frame_width = (float)video_decoder->width;
		const float frame_height = (float)video_decoder->height;
		const float scale_w = (float)mWidth / frame_width;
		const float scale_h = (float)mHeight / frame_height;
		const float scale = SDL_max(scale_w, scale_h);
		SDL_FRect dstrect;
		dstrect.w = frame_width * scale;
		dstrect.h = frame_height * scale;
		dstrect.x = ((float)mWidth - dstrect.w) / 2;
		dstrect.y = ((float)mHeight - dstrect.h) / 2;

		if (!SDL_SetRenderDrawColor(aVideoRenderer, bgColor.mRed, bgColor.mGreen, bgColor.mBlue, bgColor.mAlpha) ||
			!SDL_RenderClear(aVideoRenderer))
		{
			TodTrace("Video frame presentation failed: %s\n", SDL_GetError());
			return false;
		}

		const float anHDRScale = isUsingSDRPresentation ? 1.0f : GetHDRPaperWhiteScale();
		if (!SDL_SetRenderColorScale(aVideoRenderer, anHDRScale))
		{
			TodTrace("Video HDR paper-white setup failed: %s\n", SDL_GetError());
			return false;
		}
		const bool aRenderedFrame = SDL_RenderTexture(aVideoRenderer, texture, nullptr, &dstrect);
		const bool aRestoredColorScale = SDL_SetRenderColorScale(aVideoRenderer, 1.0f);
		if (!aRenderedFrame || !aRestoredColorScale || !SDL_RenderPresent(aVideoRenderer))
		{
			TodTrace("Video frame presentation failed: %s\n", SDL_GetError());
			return false;
		}
		return true;
	};

	if (!SDL_SetRenderTarget(aVideoRenderer, nullptr) ||
		!SDL_SetRenderDrawColor(aVideoRenderer, bgColor.mRed, bgColor.mGreen, bgColor.mBlue, bgColor.mAlpha) ||
		!SDL_RenderClear(aVideoRenderer) ||
		!SDL_RenderPresent(aVideoRenderer))
	{
		TodTrace("Initial video presentation failed: %s\n", SDL_GetError());
		videoFailed = true;
	}

	auto processVideoEvents = [&]() -> bool
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				willShutdown = true;
				break;
			case SDL_EVENT_RENDER_TARGETS_RESET:
			case SDL_EVENT_RENDER_DEVICE_RESET:
			case SDL_EVENT_RENDER_DEVICE_LOST:
				if (isUsingSDRPresentation && event.render.windowID == aVideoWindowId)
				{
					TodTrace("The auxiliary video renderer was reset; ending video playback safely.\n");
					videoFailed = true;
					mIsPlayingVideo = false;
				}
				else if (event.render.windowID == aMainWindowId)
				{
					TodTrace("The main graphics renderer reset during video playback; shutting down safely.\n");
					willShutdown = true;
					mIsPlayingVideo = false;
				}
				else
				{
					TodTrace(
						"Ignoring a graphics reset for unrelated window %u during video playback.\n",
						(unsigned int)event.render.windowID
					);
				}
				break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				if (event.window.windowID == aMainWindowId)
				{
					willShutdown = true;
					mIsPlayingVideo = false;
				}
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				if (event.window.windowID == aMainWindowId)
				{
					mActive = true;
					RehupFocus();
					EnforceCursor();
				}
				break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
				if (event.window.windowID == aMainWindowId)
				{
					mActive = false;
					RehupFocus();
				}
				break;
			case SDL_EVENT_WINDOW_MOVED:
			case SDL_EVENT_WINDOW_RESIZED:
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
			case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
			case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
			case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
				if (isUsingSDRPresentation &&
					event.window.windowID == aMainWindowId &&
					event.window.timestamp >= aVideoPresentationReadyTimestamp)
				{
					TodTrace("The main window changed while the SDR video surface was active; ending video playback safely.\n");
					mIsPlayingVideo = false;
				}
				break;
			case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
				if (!isUsingSDRPresentation &&
					event.window.windowID == aMainWindowId &&
					event.window.timestamp >= aVideoPresentationReadyTimestamp &&
					ShouldUseSDRVideoPresentation(mSDLRenderer))
				{
					TodTrace("HDR became active during direct video playback; ending the video before switching presentation paths.\n");
					mIsPlayingVideo = false;
				}
				break;
			case SDL_EVENT_KEY_DOWN:
			{
				if (event.key.windowID != aMainWindowId && event.key.windowID != aVideoWindowId)
					break;
				mLastUserInputTick = mLastTimerTime;
				if (mDebugKeysEnabled)
				{
					if (DebugKeyDown(GetKeyCodeFromCodeSDL(event.key.key)))
						break;
				}
				else
				{
					KeyCode theKey = GetKeyCodeFromCodeSDL(event.key.key);
					if (theKey == KEYCODE_F10)
					{
						TakeScreenshot();
						break;
					}
					else if (theKey == KeyCode::KEYCODE_F11)
					{
						// A fullscreen transition can invalidate the renderer while the
						// video decoder owns the presentation loop. Defer it until cleanup.
						toggleFullscreenAfterVideo = !toggleFullscreenAfterVideo;
						break;
					}
				}

				if (isSkipable && event.key.key == SDLK_ESCAPE)
				{
					mIsPlayingVideo = false;
					break;
				}

				int theChar = GetKeyCodeFromCodeSDL(event.key.key);

				if ((theChar < KEYCODE_ASCIIBEGIN || theChar > KEYCODE_ASCIIEND) && (theChar < KEYCODE_ASCIIBEGIN2 || theChar > KEYCODE_ASCIIEND2))
				{
					theChar = -1;
				}

				switch (event.key.key)
				{
				case SDLK_KP_PLUS:   theChar = '+'; break;
				case SDLK_KP_MINUS:  theChar = '-'; break;
				case SDLK_KP_MULTIPLY: theChar = '*'; break;
				case SDLK_SLASH:
				case SDLK_KP_DIVIDE: theChar = '/'; break;
				case SDLK_KP_PERIOD: theChar = '.'; break;

				case SDLK_KP_0: theChar = '0'; break;
				case SDLK_KP_1: theChar = '1'; break;
				case SDLK_KP_2: theChar = '2'; break;
				case SDLK_KP_3: theChar = '3'; break;
				case SDLK_KP_4: theChar = '4'; break;
				case SDLK_KP_5: theChar = '5'; break;
				case SDLK_KP_6: theChar = '6'; break;
				case SDLK_KP_7: theChar = '7'; break;
				case SDLK_KP_8: theChar = '8'; break;
				case SDLK_KP_9: theChar = '9'; break;
				}

				if (theChar == 'D' && (mWidgetManager != NULL) && (mWidgetManager->mKeyDown[KEYCODE_CONTROL]) && (mWidgetManager->mKeyDown[KEYCODE_MENU]))
				{
					PlaySoundA("c:\\windows\\media\\Windows XP Menu Command.wav", NULL, SND_ASYNC);
					mDebugKeysEnabled = !mDebugKeysEnabled;
				}

				mWidgetManager->KeyDown(GetKeyCodeFromCodeSDL(event.key.key));

				if (!SDL_TextInputActive(mSDLWindow)) {

					bool shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;
					bool caps = (event.key.mod & SDL_KMOD_CAPS) != 0;

					SexyChar c = theChar;

					if (isalpha(c))
					{
						if (shift ^ caps)
							c = toupper(c);
						else
							c = tolower(c);
					}
					mWidgetManager->KeyChar(c);
				}

				break;
			}
			case SDL_EVENT_KEY_UP:
				if (event.key.windowID != aMainWindowId && event.key.windowID != aVideoWindowId)
					break;
				mLastUserInputTick = mLastTimerTime;
				mWidgetManager->KeyUp(GetKeyCodeFromCodeSDL(event.key.key));
				break;
			}
		}
		return mIsPlayingVideo && !willShutdown;
	};

	bool audioPlaybackReady = audio_playback_stream != nullptr;
	auto queueDecodedAudioFrames = [&]() -> bool
	{
		int aReceiveResult = 0;
		while ((aReceiveResult = avcodec_receive_frame(audio_decoder, frame)) == 0)
		{
			if (!audioPlaybackReady)
				continue;

			bool queued = false;
			if (frame->format == AV_SAMPLE_FMT_FLTP)
			{
				queued = SDL_PutAudioStreamPlanarData(
					audio_playback_stream,
					(const void* const*)frame->extended_data,
					frame->ch_layout.nb_channels,
					frame->nb_samples
				);
			}
			else if (frame->format == AV_SAMPLE_FMT_FLT)
			{
				queued = SDL_PutAudioStreamData(
					audio_playback_stream,
					frame->data[0],
					frame->nb_samples * frame->ch_layout.nb_channels * (int)sizeof(float)
				);
			}
			else
			{
				TodTrace("Unsupported decoded audio format %d. Continuing without video audio.\n", frame->format);
				audioPlaybackReady = false;
				SDL_ClearAudioStream(audio_playback_stream);
				continue;
			}

			if (!queued)
			{
				TodTrace("Video audio upload failed: %s. Continuing without audio.\n", SDL_GetError());
				audioPlaybackReady = false;
				SDL_ClearAudioStream(audio_playback_stream);
			}
		}

		if (aReceiveResult != AVERROR(EAGAIN) && aReceiveResult != AVERROR_EOF)
		{
			TodTrace("Audio decoder failed while receiving a frame: %d\n", aReceiveResult);
			return false;
		}
		return true;
	};

	// Prebuffer the complete, short intro audio while its device remains paused.
	// Keeping compressed video packets is inexpensive and ensures timed video
	// presentation can never prevent the demuxer from feeding audio.
	int aReadResult = 0;
	while (!videoFailed && (aReadResult = av_read_frame(format_context, packet)) >= 0)
	{
		if (packet->stream_index == video_stream_index)
		{
			AVPacket* aVideoPacket = av_packet_clone(packet);
			if (aVideoPacket == nullptr)
			{
				TodTrace("Unable to buffer a video packet.\n");
				videoFailed = true;
			}
			else
			{
				videoPackets.push_back(aVideoPacket);
			}
		}
		else if (packet->stream_index == audio_stream_index)
		{
			const int aSendResult = avcodec_send_packet(audio_decoder, packet);
			if (aSendResult < 0)
			{
				TodTrace("Audio decoder rejected a packet: %d\n", aSendResult);
				videoFailed = true;
			}
			else if (!queueDecodedAudioFrames())
			{
				videoFailed = true;
			}
		}

		av_packet_unref(packet);
	}

	if (!videoFailed && aReadResult != AVERROR_EOF)
	{
		TodTrace("Video demux failed: %d\n", aReadResult);
		videoFailed = true;
	}

	if (!videoFailed)
	{
		const int anAudioFlushResult = avcodec_send_packet(audio_decoder, nullptr);
		if (anAudioFlushResult >= 0 || anAudioFlushResult == AVERROR_EOF)
		{
			if (!queueDecodedAudioFrames())
				videoFailed = true;
		}
		else
		{
			TodTrace("Audio decoder flush failed: %d\n", anAudioFlushResult);
			videoFailed = true;
		}
	}

	if (!videoFailed && audioPlaybackReady && !SDL_FlushAudioStream(audio_playback_stream))
	{
		TodTrace("Video audio flush failed: %s. Continuing without audio.\n", SDL_GetError());
		audioPlaybackReady = false;
		SDL_ClearAudioStream(audio_playback_stream);
	}

	if (!videoFailed && videoPackets.empty())
	{
		TodTrace("Video contained no decodable packets.\n");
		videoFailed = true;
	}

	if (!videoFailed && audioPlaybackReady && !SDL_ResumeAudioStreamDevice(audio_playback_stream))
	{
		TodTrace("Video audio playback failed to start: %s. Continuing without audio.\n", SDL_GetError());
		audioPlaybackReady = false;
	}
	if (!videoFailed)
		start_ns = SDL_GetTicksNS();

	for (size_t aPacketIndex = 0;
		aPacketIndex < videoPackets.size() && !videoFailed && mIsPlayingVideo && !willShutdown;
		aPacketIndex++)
	{
		mLastUserInputTick = mLastTimerTime;
		if (!processVideoEvents())
			break;

		AVPacket* aVideoPacket = videoPackets[aPacketIndex];
		videoPackets[aPacketIndex] = nullptr;
		const int aSendResult = avcodec_send_packet(video_decoder, aVideoPacket);
		av_packet_free(&aVideoPacket);
		if (aSendResult < 0)
		{
			TodTrace("Video decoder rejected a packet: %d\n", aSendResult);
			videoFailed = true;
			break;
		}

		int aReceiveResult = 0;
		while ((aReceiveResult = avcodec_receive_frame(video_decoder, frame)) == 0)
		{
			if (!presentVideoFrame(frame))
			{
				videoFailed = true;
				break;
			}
		}
		if (!videoFailed && aReceiveResult != AVERROR(EAGAIN) && aReceiveResult != AVERROR_EOF)
		{
			TodTrace("Video decoder failed while receiving a frame: %d\n", aReceiveResult);
			videoFailed = true;
		}
	}

	if (!videoFailed && mIsPlayingVideo && !willShutdown)
	{
		const int aFlushResult = avcodec_send_packet(video_decoder, nullptr);
		if (aFlushResult >= 0 || aFlushResult == AVERROR_EOF)
		{
			while (avcodec_receive_frame(video_decoder, frame) == 0)
			{
				if (!presentVideoFrame(frame))
				{
					videoFailed = true;
					break;
				}
			}
		}
		else
		{
			TodTrace("Video decoder flush failed: %d\n", aFlushResult);
			videoFailed = true;
		}
	}

	if (!videoFailed && mIsPlayingVideo && !willShutdown && format_context->duration > 0)
	{
		const double aPlaybackDurationSeconds = (double)format_context->duration / AV_TIME_BASE;
		while (processVideoEvents())
		{
			const double anElapsedSeconds = (double)(SDL_GetTicksNS() - start_ns) / SDL_NS_PER_SECOND;
			const double aRemainingSeconds = aPlaybackDurationSeconds - anElapsedSeconds;
			if (aRemainingSeconds <= 0.0)
				break;
			const double aWaitSeconds = SDL_min(aRemainingSeconds, 0.002);
			SDL_DelayPrecise((Uint64)(aWaitSeconds * SDL_NS_PER_SECOND));
		}
	}

	for (AVPacket*& aVideoPacket : videoPackets)
		av_packet_free(&aVideoPacket);
	
	if (!SDL_SetRenderColorScale(aVideoRenderer, 1.0f))
		TodTrace("Video HDR paper-white cleanup failed: %s\n", SDL_GetError());
	if (!isUsingSDRPresentation &&
		!SDL_SetRenderVSync(aVideoRenderer, havePreviousVsync ? previousVsync : (mEnableVsync ? 1 : 0)))
		TodTrace("Video VSync restore failed: %s\n", SDL_GetError());
	SDL_DestroyTexture(texture);
	anSDRPresentation.Destroy();
	sws_freeContext(videoScaler);
	av_free(bgraPixels);
	SDL_DestroyAudioStream(audio_playback_stream);
	av_frame_free(&frame);
	av_packet_free(&packet);
	avcodec_free_context(&audio_decoder);
	avcodec_free_context(&video_decoder);
	avformat_close_input(&format_context);

	mIsPlayingVideo = false;
	mWidgetManager->MarkAllDirty();
	if (toggleFullscreenAfterVideo && !willShutdown)
		SwitchScreenMode(!mIsWindowed, true);

	SDL_ShowCursor();

	if (willShutdown) Shutdown();

	return !videoFailed;
}

void LawnApp::MakeWindow()
{
	//SexyAppBase::MakeWindow();
	if (mSDLWindow != nullptr) {
		DestroyHDRToneMapTexture();
		SDL_DestroyRenderer(mSDLRenderer);
		SDL_DestroyWindow(mSDLWindow);
	}

	if (mDDInterface == nullptr) {
		mDDInterface = new DDInterface(this);
	}

	if (IsScreenSaver())
	{
		mTitle = _S("Zen Garden");
		mIsWindowed = false;
		mFullScreenWindow = true;
	}
	else if (IsPreviewSaver())
	{
		mTitle = _S("Zen Garden Preview");
		mIsWindowed = true;
		mFullScreenWindow = false;
	}
	else if (IsParticleEditor())
	{
		mTitle = _S("Particle Editor v0.1 by @inliothixie");
		mIsWindowed = true;
		mFullScreenWindow = false;
	}

#define _WIDE_SCREEN
#ifdef _ULTRA_WIDESCREEN
	mWidth = 1280;
	mHeight = 720;

	mDDInterface->mWideScreenOffsetX = 240;
	mDDInterface->mWideScreenOffsetY = 60;
#elif defined(_WIDE_SCREEN)
	mWidth = 1066;
	mHeight = 600;

	mDDInterface->mWideScreenOffsetX = 133;
#endif

	gBoardBounds = Rect{ 0, 0, mWidth, mHeight };

	unsigned long long windowFlags = 0UL;
	if (!IsParticleEditor()) windowFlags |= SDL_WINDOW_RESIZABLE;
	if (IsScreenSaver() || !mIsWindowed) windowFlags |= SDL_WINDOW_FULLSCREEN;
	windowFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

	SDL_RendererLogicalPresentation presentationMode;

	mSDLWindow = SDL_CreateWindow(mTitle.c_str(), mWidth, mHeight, windowFlags);
	ConfigureFullscreenDisplayMode();
	if (mSDLWindow != nullptr && (windowFlags & SDL_WINDOW_FULLSCREEN) != 0 && !SDL_SyncWindow(mSDLWindow))
		TodTrace("Initial fullscreen synchronization timed out: %s\n", SDL_GetError());
	mNativeHDRRenderer = false;
	mHDRToneMapUnavailable = false;
	mSDLRenderer = CreateLawnRenderer(mSDLWindow, mEnableNativeHDR, mEnableVsync);
	if (mSDLRenderer == nullptr && mEnableNativeHDR)
	{
		TodTrace("Native HDR renderer creation failed: %s. Falling back to SDR.\n", SDL_GetError());
		mSDLRenderer = CreateLawnRenderer(mSDLWindow, false, mEnableVsync);
	}
	if (mSDLRenderer == nullptr)
	{
		TodTrace("Renderer creation with properties failed: %s. Falling back to the default renderer.\n", SDL_GetError());
		mSDLRenderer = SDL_CreateRenderer(mSDLWindow, nullptr);
	}

	if (mSDLRenderer != nullptr)
	{
		SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mSDLRenderer);
		SDL_Colorspace anOutputColorspace = (SDL_Colorspace)SDL_GetNumberProperty(
			aRendererProperties,
			SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
			SDL_COLORSPACE_UNKNOWN
		);
		mNativeHDRRenderer = anOutputColorspace == SDL_COLORSPACE_SRGB_LINEAR;
		TodTrace(
			"Renderer: %s, output colorspace: 0x%X, HDR active: %s, SDR white: %.3f, HDR headroom: %.3f\n",
			SDL_GetRendererName(mSDLRenderer),
			(unsigned int)anOutputColorspace,
			SDL_GetBooleanProperty(aRendererProperties, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false) ? "yes" : "no",
			SDL_GetFloatProperty(aRendererProperties, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, 1.0f),
			SDL_GetFloatProperty(aRendererProperties, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 1.0f)
		);
	}
	SDL_SetRenderVSync(mSDLRenderer, mEnableVsync);
	ApplyLogicalPresentationMode();
	mHWnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(mSDLWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

	mWidgetManager->mWidth = mWidth;
	mWidgetManager->mHeight = mHeight;

	mWidgetManager->mImage = new SDL3Image(mSDLRenderer);
	mWidgetManager->mImage->mWidth = mWidth;
	mWidgetManager->mImage->mHeight = mHeight;
	mWidgetManager->mImage->mD3DData = SDL3Image::CreateRenderTarget(
		mSDLRenderer,
		mWidgetManager->mImage->mWidth,
		mWidgetManager->mImage->mHeight
	);
	SDL_Texture* aScreenTexture = (SDL_Texture*)mWidgetManager->mImage->mD3DData;
	if (aScreenTexture != nullptr)
	{
		SDL_SetTextureBlendMode(aScreenTexture, SDL_BLENDMODE_NONE);
		SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(mSDLRenderer);
		SDL_SetRenderTarget(mSDLRenderer, aScreenTexture);
		SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mSDLRenderer);
		SDL_SetRenderTarget(mSDLRenderer, anOldRenderTarget);
	}
	else
	{
		TodTrace("Persistent screen texture creation failed: %s. Using full-frame direct rendering.\n", SDL_GetError());
	}
	mWidgetManager->MarkAllDirty();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	style.ScaleAllSizes(main_scale);      
	style.FontScaleDpi = main_scale;

	ImGui_ImplSDL3_InitForSDLRenderer(mSDLWindow, mSDLRenderer);
	ImGui_ImplSDLRenderer3_Init(mSDLRenderer);

	SDL_RaiseWindow(mSDLWindow);
}

bool LawnApp::IsNativeHDRActive() const
{
	if (!mNativeHDRRenderer || mSDLRenderer == nullptr)
		return false;

	SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mSDLRenderer);
	return SDL_GetBooleanProperty(aRendererProperties, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false);
}

float LawnApp::GetHDRPaperWhiteScale() const
{
	if (!IsNativeHDRActive())
		return 1.0f;

	const float aRequestedScale = (float)std::clamp(mHDRPaperWhitePercent, 50, 200) / 100.0f;
	const SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mSDLRenderer);
	const float aReportedHeadroom = SDL_GetFloatProperty(
		aRendererProperties,
		SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT,
		1.0f
	);
	const float aHeadroom = aReportedHeadroom < 1.0f ? 1.0f : aReportedHeadroom;
	return aRequestedScale > aHeadroom ? aHeadroom : aRequestedScale;
}

float LawnApp::GetHDRCompositeScale() const
{
	if (!IsNativeHDRActive())
		return 1.0f;

	const float anExposureScale = std::exp2((float)std::clamp(mHDRExposureTenthsEV, -20, 20) / 10.0f);
	const float aPaperWhiteScale = mHDRAdaptiveToneMapping
		? (float)std::clamp(mHDRPaperWhitePercent, 50, 200) / 100.0f
		: GetHDRPaperWhiteScale();
	return std::clamp(aPaperWhiteScale * anExposureScale, 0.125f, 8.0f);
}

void LawnApp::DestroyHDRToneMapTexture()
{
	if (mHDRToneMapTexture != nullptr)
	{
		SDL_DestroyTexture(mHDRToneMapTexture);
		mHDRToneMapTexture = nullptr;
	}
	mHDRToneMapTextureScaleMilli = 0;
}

void LawnApp::ApplyLogicalPresentationMode()
{
	if (mSDLRenderer == nullptr)
		return;

	const SDL_RendererLogicalPresentation aPresentationMode = mUseIntegerScaling
		? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
		: SDL_LOGICAL_PRESENTATION_LETTERBOX;
	if (!SDL_SetRenderLogicalPresentation(mSDLRenderer, mWidth, mHeight, aPresentationMode))
		TodTrace("Logical presentation setup failed: %s\n", SDL_GetError());

	if (mWidgetManager != nullptr)
	{
		mWidgetManager->MarkAllDirty();
		mHasPendingDraw = true;
	}
}

bool LawnApp::ConfigureFullscreenDisplayMode()
{
	if (mSDLWindow == nullptr)
		return false;

	if (!mUseExclusiveFullscreen)
	{
		if (!SDL_SetWindowFullscreenMode(mSDLWindow, nullptr))
		{
			TodTrace("Borderless fullscreen setup failed: %s\n", SDL_GetError());
			return false;
		}
		return true;
	}

	const SDL_DisplayID aDisplay = SDL_GetDisplayForWindow(mSDLWindow);
	const SDL_DisplayMode* aDesktopMode = aDisplay == 0 ? nullptr : SDL_GetDesktopDisplayMode(aDisplay);
	int aModeCount = 0;
	SDL_DisplayMode** aModes = aDisplay == 0 ? nullptr : SDL_GetFullscreenDisplayModes(aDisplay, &aModeCount);
	const SDL_DisplayMode* aBestMode = nullptr;
	double aBestScore = 1.0e30;
	const double aDesiredRefresh = mPreferredRefreshRateMilliHz > 0
		? (double)mPreferredRefreshRateMilliHz / 1000.0
		: (aDesktopMode == nullptr ? 0.0 : aDesktopMode->refresh_rate);

	if (aDesktopMode != nullptr && aModes != nullptr)
	{
		for (int i = 0; i < aModeCount; i++)
		{
			const SDL_DisplayMode* aMode = aModes[i];
			if (aMode == nullptr ||
				aMode->w != aDesktopMode->w ||
				aMode->h != aDesktopMode->h ||
				aMode->format != aDesktopMode->format ||
				std::abs(aMode->pixel_density - aDesktopMode->pixel_density) > 0.01f ||
				aMode->refresh_rate <= 0.0f)
				continue;

			const double aRefreshDelta = std::abs((double)aMode->refresh_rate - aDesiredRefresh);
			const double aFormatPenalty = aMode->format == aDesktopMode->format ? 0.0 : 10.0;
			const double aDensityPenalty = std::abs((double)aMode->pixel_density - aDesktopMode->pixel_density);
			const double aScore = aRefreshDelta * 1000.0 + aFormatPenalty + aDensityPenalty;
			if (aScore < aBestScore)
			{
				aBestScore = aScore;
				aBestMode = aMode;
			}
		}
	}

	const bool aRequestedRateIsAvailable = aBestMode != nullptr &&
		(mPreferredRefreshRateMilliHz == 0 ||
		 std::abs((double)aBestMode->refresh_rate * 1000.0 - mPreferredRefreshRateMilliHz) <= 100.0);
	bool aConfigured = false;
	if (aRequestedRateIsAvailable)
	{
		aConfigured = SDL_SetWindowFullscreenMode(mSDLWindow, aBestMode);
		if (aConfigured)
		{
			TodTrace(
				"Exclusive fullscreen mode configured: %dx%d at %.3f Hz.\n",
				aBestMode->w,
				aBestMode->h,
				aBestMode->refresh_rate
			);
		}
	}
	SDL_free(aModes);

	if (!aConfigured)
	{
		TodTrace("Requested exclusive fullscreen mode is unavailable: %s. Falling back to borderless desktop.\n", SDL_GetError());
		if (!SDL_SetWindowFullscreenMode(mSDLWindow, nullptr))
			TodTrace("Borderless fullscreen fallback failed: %s\n", SDL_GetError());
		mUseExclusiveFullscreen = false;
	}
	return aConfigured;
}

bool LawnApp::DrawDirtyStuff()
{
	if (mIsPlayingVideo) return true;
	if (IsParticleEditor() && mParticleScreen) mParticleScreen->ImGuiDraw();
	SDL_Texture* anOldRenderTarget = SDL_GetRenderTarget(mSDLRenderer);
	SDL_Texture* aScreenTexture = (SDL_Texture*)mWidgetManager->mImage->mD3DData;
	if (aScreenTexture != nullptr && !SDL_SetRenderTarget(mSDLRenderer, aScreenTexture))
	{
		TodTrace("Persistent screen texture bind failed: %s. Using full-frame direct rendering.\n", SDL_GetError());
		if (anOldRenderTarget == aScreenTexture)
			anOldRenderTarget = nullptr;
		SDL_DestroyTexture(aScreenTexture);
		mWidgetManager->mImage->mD3DData = nullptr;
		DestroyHDRToneMapTexture();
		aScreenTexture = nullptr;
	}
	if (aScreenTexture == nullptr)
	{
		SDL_SetRenderTarget(mSDLRenderer, nullptr);
		SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mSDLRenderer);
		SDL_SetRenderColorScale(mSDLRenderer, GetHDRCompositeScale());
		mWidgetManager->MarkAllDirty();
	}
	bool drewStuff = SexyAppBase::DrawDirtyStuff();
	if (aScreenTexture == nullptr)
		SDL_SetRenderColorScale(mSDLRenderer, 1.0f);
	SDL_SetRenderTarget(mSDLRenderer, anOldRenderTarget);
	return drewStuff;
}

void LawnApp::Redraw(Rect* theClipRect)
{
	if (mIsPlayingVideo || mMinimized) return;
	//SexyAppBase::Redraw(theClipRect);
	SDL_Texture* aScreenTexture = (SDL_Texture*)mWidgetManager->mImage->mD3DData;
	SDL_SetRenderTarget(mSDLRenderer, nullptr);
	if (aScreenTexture != nullptr)
	{
		float aCompositeScale = GetHDRCompositeScale();
		SDL_Texture* aCompositeTexture = aScreenTexture;
		bool isUsingAdaptiveToneMapping = false;

		if (mHDRAdaptiveToneMapping && !mHDRToneMapUnavailable && IsNativeHDRActive())
		{
			const SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mSDLRenderer);
			float aDisplayHeadroom = SDL_GetFloatProperty(
				aRendererProperties,
				SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT,
				1.0f
			);
			if (!std::isfinite(aDisplayHeadroom) || aDisplayHeadroom < 1.0f)
				aDisplayHeadroom = 1.0f;

			if (aCompositeScale > aDisplayHeadroom + 0.001f)
			{
				const int aScaleMilli = (int)std::lround(aCompositeScale * 1000.0f);
				if (mHDRToneMapTexture == nullptr || mHDRToneMapTextureScaleMilli != aScaleMilli)
				{
					SDL_PropertiesID aProperties = SDL_CreateProperties();
					SDL_Texture* aNewToneMapTexture = nullptr;
					bool aToneMapTargetMetadataWasRejected = false;
					if (aProperties != 0)
					{
						const bool aPropertiesWereConfigured =
							SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_RGBA64_FLOAT) &&
							SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_TARGET) &&
							SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, mWidth) &&
							SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, mHeight) &&
							SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, SDL_COLORSPACE_SRGB_LINEAR) &&
							SDL_SetFloatProperty(aProperties, SDL_PROP_TEXTURE_CREATE_SDR_WHITE_POINT_FLOAT, 1.0f) &&
							SDL_SetFloatProperty(aProperties, SDL_PROP_TEXTURE_CREATE_HDR_HEADROOM_FLOAT, aCompositeScale);
						if (aPropertiesWereConfigured)
							aNewToneMapTexture = SDL_CreateTextureWithProperties(mSDLRenderer, aProperties);
						SDL_DestroyProperties(aProperties);
					}

					if (aNewToneMapTexture != nullptr)
					{
						const SDL_PropertiesID aTextureProperties = SDL_GetTextureProperties(aNewToneMapTexture);
						const SDL_Colorspace aTextureColorspace = (SDL_Colorspace)SDL_GetNumberProperty(
							aTextureProperties,
							SDL_PROP_TEXTURE_COLORSPACE_NUMBER,
							SDL_COLORSPACE_UNKNOWN
						);
						const SDL_PixelFormat aTextureFormat = (SDL_PixelFormat)SDL_GetNumberProperty(
							aTextureProperties,
							SDL_PROP_TEXTURE_FORMAT_NUMBER,
							SDL_PIXELFORMAT_UNKNOWN
						);
						const float aTextureWhitePoint = SDL_GetFloatProperty(
							aTextureProperties,
							SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT,
							0.0f
						);
						const float aTextureHeadroom = SDL_GetFloatProperty(
							aTextureProperties,
							SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT,
							0.0f
						);
						const bool aTextureIsValid =
							aTextureColorspace == SDL_COLORSPACE_SRGB_LINEAR &&
							aTextureFormat == SDL_PIXELFORMAT_RGBA64_FLOAT &&
							std::abs(aTextureWhitePoint - 1.0f) <= 0.001f &&
							std::abs(aTextureHeadroom - aCompositeScale) <= 0.01f &&
							SDL_SetTextureBlendMode(aNewToneMapTexture, SDL_BLENDMODE_NONE);

						if (aTextureIsValid)
						{
							SDL_ScaleMode aScaleMode = SDL_SCALEMODE_LINEAR;
							if (SDL_GetTextureScaleMode(aScreenTexture, &aScaleMode))
								SDL_SetTextureScaleMode(aNewToneMapTexture, aScaleMode);
							DestroyHDRToneMapTexture();
							mHDRToneMapTexture = aNewToneMapTexture;
							mHDRToneMapTextureScaleMilli = aScaleMilli;
						}
						else
						{
							TodTrace("HDR tone-map target metadata is unsupported. Using clipped HDR output.\n");
							SDL_DestroyTexture(aNewToneMapTexture);
							aNewToneMapTexture = nullptr;
							aToneMapTargetMetadataWasRejected = true;
						}
					}
					if (aNewToneMapTexture == nullptr)
					{
						if (!aToneMapTargetMetadataWasRejected)
							TodTrace("HDR tone-map target creation failed: %s. Using clipped HDR output.\n", SDL_GetError());
						DestroyHDRToneMapTexture();
						mHDRToneMapUnavailable = true;
					}
				}

				if (mHDRToneMapTexture != nullptr)
				{
					SDL_SetTextureColorMod(aScreenTexture, 255, 255, 255);
					SDL_SetTextureAlphaMod(aScreenTexture, 255);
					SDL_SetTextureBlendMode(aScreenTexture, SDL_BLENDMODE_NONE);
					const bool aTargetWasSet = SDL_SetRenderTarget(mSDLRenderer, mHDRToneMapTexture);
					const bool aScaleWasSet = aTargetWasSet && SDL_SetRenderColorScale(mSDLRenderer, aCompositeScale);
					SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
					const bool aTargetWasCleared = aScaleWasSet && SDL_RenderClear(mSDLRenderer);
					const bool aToneMapSourceWasDrawn = aTargetWasCleared && SDL_RenderTexture(mSDLRenderer, aScreenTexture, nullptr, nullptr);
					const bool aScaleWasRestored = SDL_SetRenderColorScale(mSDLRenderer, 1.0f);
					const bool aWindowTargetWasRestored = SDL_SetRenderTarget(mSDLRenderer, nullptr);

					if (aTargetWasSet && aScaleWasSet && aTargetWasCleared && aToneMapSourceWasDrawn &&
						aScaleWasRestored && aWindowTargetWasRestored)
					{
						aCompositeTexture = mHDRToneMapTexture;
						aCompositeScale = 1.0f;
						isUsingAdaptiveToneMapping = true;
					}
					else
					{
						TodTrace("HDR tone-map pass failed: %s. Using clipped HDR output.\n", SDL_GetError());
						SDL_SetRenderColorScale(mSDLRenderer, 1.0f);
						SDL_SetRenderTarget(mSDLRenderer, nullptr);
						DestroyHDRToneMapTexture();
						mHDRToneMapUnavailable = true;
					}
				}
			}
		}

		SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mSDLRenderer);
		SDL_SetTextureColorMod(aCompositeTexture, 255, 255, 255);
		SDL_SetTextureAlphaMod(aCompositeTexture, 255);
		SDL_SetTextureBlendMode(aCompositeTexture, SDL_BLENDMODE_NONE);
		const bool aScaleWasSet = SDL_SetRenderColorScale(mSDLRenderer, aCompositeScale);
		bool aCompositeSucceeded = aScaleWasSet && SDL_RenderTexture(mSDLRenderer, aCompositeTexture, nullptr, nullptr);
		const bool aScaleWasRestored = SDL_SetRenderColorScale(mSDLRenderer, 1.0f);
		if (!aScaleWasSet)
			TodTrace("HDR composite scale setup failed: %s\n", SDL_GetError());
		if (!aScaleWasRestored)
			TodTrace("HDR composite scale restore failed: %s\n", SDL_GetError());

		if (!aCompositeSucceeded && isUsingAdaptiveToneMapping)
		{
			TodTrace("Adaptive HDR composite failed: %s. Retrying clipped HDR output.\n", SDL_GetError());
			DestroyHDRToneMapTexture();
			mHDRToneMapUnavailable = true;
			SDL_SetRenderTarget(mSDLRenderer, nullptr);
			SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(mSDLRenderer);
			SDL_SetTextureColorMod(aScreenTexture, 255, 255, 255);
			SDL_SetTextureAlphaMod(aScreenTexture, 255);
			SDL_SetTextureBlendMode(aScreenTexture, SDL_BLENDMODE_NONE);
			const bool aFallbackScaleWasSet = SDL_SetRenderColorScale(mSDLRenderer, GetHDRCompositeScale());
			aCompositeSucceeded = aFallbackScaleWasSet && SDL_RenderTexture(mSDLRenderer, aScreenTexture, nullptr, nullptr);
			SDL_SetRenderColorScale(mSDLRenderer, 1.0f);
		}

		if (!aCompositeSucceeded)
		{
			TodTrace("Screen composite failed: %s. Switching to full-frame direct rendering.\n", SDL_GetError());
			DestroyHDRToneMapTexture();
			SDL_DestroyTexture(aScreenTexture);
			mWidgetManager->mImage->mD3DData = nullptr;
			mWidgetManager->MarkAllDirty();
			mHasPendingDraw = true;
			return;
		}
	}

	if (auto drawData = ImGui::GetDrawData())
	{
		ImGui_ImplSDLRenderer3_RenderDrawData(drawData, mSDLRenderer);
	}
	if (!SDL_RenderPresent(mSDLRenderer))
	{
		TodTrace("Screen present failed: %s\n", SDL_GetError());
		mWidgetManager->MarkAllDirty();
		mHasPendingDraw = true;
	}
}

static int mouseMoveCount = 0;
static int lastMouseX = -1;
static int lastMouseY = -1;

bool IsRealMouseMove(const SDL_Event& e)
{
	if (lastMouseX == -1)
	{
		lastMouseX = e.motion.x;
		lastMouseY = e.motion.y;
		return false;
	}

	if (e.motion.x != lastMouseX || e.motion.y != lastMouseY)
	{
		lastMouseX = e.motion.x;
		lastMouseY = e.motion.y;
		mouseMoveCount++;
	}

	return mouseMoveCount >= 4;
}

bool LawnApp::UpdateAppStep(bool* updated)
{
	if (updated != nullptr)
		*updated = false;

	if (mExitToTop)
		return false;

	if (mUpdateAppState == UPDATESTATE_PROCESS_DONE)
		mUpdateAppState = UPDATESTATE_MESSAGES;

	mUpdateAppDepth++;

	auto buttonTrans = [](Uint8 button)
	{
		switch (button)
		{
		case SDL_BUTTON_LEFT: return 1;
		case SDL_BUTTON_RIGHT: return -1;
		case SDL_BUTTON_MIDDLE: return 3;
		}
		return 0;
	};

	if (mUpdateAppState == UPDATESTATE_MESSAGES)
	{
		SDL_Event event;
		while (!mShutdown && SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			ImGuiIO& io = ImGui::GetIO();
			bool imguiWantsMouse = io.WantCaptureMouse;
			bool imguiWantsKeyboard = io.WantCaptureKeyboard;
			
			SDL_ConvertEventToRenderCoordinates(mSDLRenderer, &event);

			if (IsScreenSaver()) {
				switch (event.type)
				{
					case SDL_EVENT_QUIT:
						Shutdown();
						break;

					case SDL_EVENT_MOUSE_MOTION:
						if (IsRealMouseMove(event))
							Shutdown();
						break;

					case SDL_EVENT_MOUSE_BUTTON_DOWN:
					case SDL_EVENT_KEY_DOWN:
					case SDL_EVENT_TEXT_INPUT:
							Shutdown();
						break;

					default:
						break;
				}
			}
			else {
				switch (event.type)
				{
				case SDL_EVENT_QUIT:
					Shutdown();
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					if ((!gInAssert) && (!mSEHOccured) && !imguiWantsMouse)
					{
						int x = event.button.x;
						int y = event.button.y;
						mWidgetManager->RemapMouse(x, y);
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->MouseMove(x, y);

						if (!mMouseIn)
						{
							mMouseIn = true;
							EnforceCursor();
						}

						mWidgetManager->MouseDown(event.button.x, event.button.y, buttonTrans(event.button.button));
					}
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if ((!gInAssert) && (!mSEHOccured) && !imguiWantsMouse)
					{
						int x = event.button.x;
						int y = event.button.y;
						mWidgetManager->RemapMouse(x, y);
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->MouseMove(x, y);

						if (!mMouseIn)
						{
							mMouseIn = true;
							EnforceCursor();
						}

						mWidgetManager->MouseUp(event.button.x, event.button.y, buttonTrans(event.button.button));
					}
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if ((!gInAssert) && (!mSEHOccured) && !imguiWantsMouse)
					{
						int x = event.motion.x;
						int y = event.motion.y;
						mWidgetManager->RemapMouse(x, y);
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->MouseMove(x, y);

						if (!mMouseIn)
						{
							mMouseIn = true;
							EnforceCursor();
						}
					}
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					if (!imguiWantsMouse)
					{
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->MouseWheel(event.wheel.y);
						
					}
					break;
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
					mActive = true;
					RehupFocus();
					EnforceCursor();
					break;
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					mActive = false;
					RehupFocus();
					break;
				case SDL_EVENT_WINDOW_MINIMIZED:
					mMinimized = true;
					mPhysMinimized = true;
					break;
				case SDL_EVENT_WINDOW_RESTORED:
					mMinimized = false;
					mPhysMinimized = false;
					ClearUpdateBacklog();
					mWidgetManager->MarkAllDirty();
					mHasPendingDraw = true;
					break;
				case SDL_EVENT_WINDOW_EXPOSED:
				case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
				case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
				case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
				case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
					mWidgetManager->MarkAllDirty();
					mHasPendingDraw = true;
					break;
				case SDL_EVENT_WINDOW_RESIZED:
				{
					float scale = 1.0f;
					int pw, ph;
					SDL_GetWindowSizeInPixels(mSDLWindow, &pw, &ph);

					if (pw >= 800 || ph >= 600) scale = max(max(static_cast<float>(pw) / 800.0f, static_cast<float>(ph) / 600.0f), 1.0f);
					SDL3Font::RebuildFonts(scale);
					mWidgetManager->MarkAllDirty();
					mHasPendingDraw = true;
					break;
				}
				case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
				{
					SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mSDLRenderer);
					TodTrace(
						"HDR state changed: active: %s, SDR white: %.3f, HDR headroom: %.3f\n",
						IsNativeHDRActive() ? "yes" : "no",
						SDL_GetFloatProperty(aRendererProperties, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, 1.0f),
						SDL_GetFloatProperty(aRendererProperties, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 1.0f)
					);
					mHDRToneMapUnavailable = false;
					mWidgetManager->MarkAllDirty();
					mHasPendingDraw = true;
					break;
				}
				case SDL_EVENT_RENDER_TARGETS_RESET:
				case SDL_EVENT_RENDER_DEVICE_RESET:
				case SDL_EVENT_RENDER_DEVICE_LOST:
				{
					const SDL_WindowID aMainWindowId = SDL_GetWindowID(mSDLWindow);
					if (event.render.windowID != aMainWindowId)
					{
						TodTrace(
							"Ignoring a graphics reset for auxiliary window %u.\n",
							(unsigned int)event.render.windowID
						);
						break;
					}
					const char* aReason = event.type == SDL_EVENT_RENDER_DEVICE_LOST ? "lost" : "reset";
					TodTrace("Graphics device %s; an orderly restart is required.\n", aReason);
					SDL_ShowSimpleMessageBox(
						SDL_MESSAGEBOX_ERROR,
						"Graphics device changed",
						"The graphics device was reset. Plants vs. Zombies must close so its graphics resources can be rebuilt safely.",
						mSDLWindow
					);
					Shutdown();
					break;
				}
				case SDL_EVENT_KEY_DOWN:
				{
					if (!imguiWantsKeyboard)
					{
						mLastUserInputTick = mLastTimerTime;
						if (mDebugKeysEnabled)
						{
							if (DebugKeyDown(GetKeyCodeFromCodeSDL(event.key.key)))
								break;
						}
						else
						{
							KeyCode theKey = GetKeyCodeFromCodeSDL(event.key.key);
							if (theKey == KEYCODE_F10)
							{
								TakeScreenshot();
								break;
							}
							else if (theKey == KeyCode::KEYCODE_F11)
							{
								gLawnApp->SwitchScreenMode(!gLawnApp->mIsWindowed, true);
								break;
							}
						}

						int theChar = GetKeyCodeFromCodeSDL(event.key.key);

						if ((theChar < KEYCODE_ASCIIBEGIN || theChar > KEYCODE_ASCIIEND) && (theChar < KEYCODE_ASCIIBEGIN2 || theChar > KEYCODE_ASCIIEND2))
						{
							theChar = -1;
						}

						{
							bool shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;
							switch (event.key.key)
							{
							case SDLK_EXCLAIM: theChar = '!'; break;
							case SDLK_AT: theChar = '@'; break;
							case SDLK_KP_HASH:   theChar = '#'; break;
							case SDLK_DOLLAR:   theChar = '$'; break;
							case SDLK_PERCENT:   theChar = '%'; break;
							case SDLK_AMPERSAND:   theChar = '&'; break;
							case SDLK_ASTERISK:   theChar = '*'; break;

							case SDLK_KP_PLUS:   theChar = '+'; break;
							case SDLK_KP_MINUS:  theChar = '-'; break;
							case SDLK_KP_MULTIPLY: theChar = '*'; break;
							case SDLK_SLASH:
							case SDLK_KP_DIVIDE: theChar = '/'; break;
							case SDLK_KP_PERIOD: theChar = '.'; break;

							case SDLK_KP_0: theChar = shift ? ')' : '0'; break;
							case SDLK_KP_1: theChar = shift ? '!' : '1'; break;
							case SDLK_KP_2: theChar = shift ? '@' : '2'; break;
							case SDLK_KP_3: theChar = shift ? '#' : '3'; break;
							case SDLK_KP_4: theChar = shift ? '$' : '4'; break;
							case SDLK_KP_5: theChar = shift ? '%' : '5'; break;
							case SDLK_KP_6: theChar = shift ? '^' : '6'; break;
							case SDLK_KP_7: theChar = shift ? '&' : '7'; break;
							case SDLK_KP_8: theChar = shift ? '*' : '8'; break;
							case SDLK_KP_9: theChar = shift ? '(' : '9'; break;
							}
						}

						if (theChar != -1 && theChar == 'D' && (mWidgetManager != NULL) && (mWidgetManager->mKeyDown[KEYCODE_CONTROL]) && (mWidgetManager->mKeyDown[KEYCODE_MENU]))
						{
							PlaySoundA("c:\\windows\\media\\Windows XP Menu Command.wav", NULL, SND_ASYNC);
							mDebugKeysEnabled = !mDebugKeysEnabled;
						}

						mWidgetManager->KeyDown(GetKeyCodeFromCodeSDL(event.key.key));

						if (theChar != -1 && !SDL_TextInputActive(mSDLWindow)) {

							bool shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;
							bool caps = (event.key.mod & SDL_KMOD_CAPS) != 0;

							SexyChar c = theChar;

							if (isalpha(c))
							{
								if (shift ^ caps)
									c = toupper(c);
								else
									c = tolower(c);
							}
							mWidgetManager->KeyChar(c);
						}	
					}
					break;
				}
				case SDL_EVENT_KEY_UP:					
					if (!imguiWantsKeyboard)
					{
						mLastUserInputTick = mLastTimerTime;
						mWidgetManager->KeyUp(GetKeyCodeFromCodeSDL(event.key.key));
					}
					break;
				case SDL_EVENT_TEXT_INPUT:
					if (!imguiWantsKeyboard)
					{
						mLastUserInputTick = mLastTimerTime;
						SexyChar aChar = event.text.text[0];

						mWidgetManager->KeyChar((SexyChar)aChar);
					}
					break;

				}
			}

			
		}
		if (mShutdown)
		{
			mUpdateAppDepth--;
			return false;
		}

		mUpdateAppState = UPDATESTATE_PROCESS_1;
	}
	else
	{
		if (mStepMode)
		{
			if (mStepMode == 2)
			{
				Sleep(mFrameTime);
				mUpdateAppState = UPDATESTATE_PROCESS_DONE;
			}
			else
			{
				mStepMode = 2;
				DoUpdateFrames();
				DoUpdateFramesF(1.0f);
				DrawDirtyStuff();
			}
		}
		else
		{
			int anOldUpdateCnt = mUpdateCount;
			Process();
			if (updated != NULL)
				*updated = mUpdateCount != anOldUpdateCnt;
		}
	}

	mUpdateAppDepth--;

	return true;
}

//0x44E8A0
bool LawnGetCloseRequest()
{
	if (gLawnApp == nullptr)
		return false;

	return gLawnApp->mCloseRequest;
}

//0x44E8C0
bool LawnHasUsedCheatKeys()
{
	return gLawnApp && gLawnApp->mPlayerInfo && gLawnApp->mPlayerInfo->mHasUsedCheatKeys;
}

//0x44EAA0
LawnApp::LawnApp()
{
	mBoard = nullptr;
	mGameSelector = nullptr;
	mChallengeScreen = nullptr;
	mSeedChooserScreen = nullptr;
	mAwardScreen = nullptr;
	mCreditScreen = nullptr;
	mLanguageScreen = nullptr;
	mParticleScreen = nullptr;
	mTitleScreen = nullptr;
	mSoundSystem = nullptr;
	mKonamiCheck = nullptr;
	mMustacheCheck = nullptr;
	mMoustacheCheck = nullptr;
	mSuperMowerCheck = nullptr;
	mSuperMowerCheck2 = nullptr;
	mFutureCheck = nullptr;
	mPinataCheck = nullptr;
	mDanceCheck = nullptr;
	mDaisyCheck = nullptr;
	mSukhbirCheck = nullptr;
	mMustacheMode = false;
	mSuperMowerMode = false;
	mFutureMode = false;
	mPinataMode = false;
	mDanceMode = false;
	mDaisyMode = false;
	mSukhbirMode = false;
	mGameScene = GameScenes::SCENE_LOADING;
	mPoolEffect = nullptr;
	mZenGarden = nullptr;
	mEffectSystem = nullptr;
	mReanimatorCache = nullptr;
	mCloseRequest = false;
	mWidth = BOARD_WIDTH;
	mHeight = BOARD_HEIGHT;
	mFullscreenBits = 32;
	mAppCounter = 0;
	mAppRandSeed = _time64(nullptr);
	mTrialType = TrialType::TRIALTYPE_NONE;
	mDebugTrialLocked = false;
	mMuteSoundsForCutscene = false;
	mMusicVolume = 0.85;
	mSfxVolume = 0.5525;
	mAutoStartLoadingThread = false;
	mDebugKeysEnabled = false;
	mProdName = "PlantsVsZombies";
	SexyString aTitleName = _S("Plants vs. Zombies");
#ifdef _DEBUG
	aTitleName += _S(" ") + StringToSexyString(mProductVersion);
	aTitleName += _S(" DEBUG");
#endif
	mTitle = aTitleName;
	mCustomCursorsEnabled = false;
	mPlayerInfo = nullptr;
	mLastLevelStats = new LevelStats();
	mFirstTimeGameSelector = true;
	mGameMode = GameMode::GAMEMODE_ADVENTURE;
	mEasyPlantingCheat = false;
	mAutoEnable3D = true;
	Tod_SWTri_AddAllDrawTriFuncs();
	mLoadingZombiesThreadCompleted = true;
	mGamesPlayed = 0;
	mMaxExecutions = 0;
	mMaxPlays = 0;
	mMaxTime = 0;
	mCompletedLoadingThreadTasks = 0;
	mProfileMgr = new ProfileMgr();
	mRegisterResourcesLoaded = false;
	mTodCheatKeys = false;
	mCrazyDaveReanimID = ReanimationID::REANIMATIONID_NULL;
	mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_OFF;
	mCrazyDaveBlinkCounter = 0;
	mCrazyDaveBlinkReanimID = ReanimationID::REANIMATIONID_NULL;
	mCrazyDaveMessageIndex = -1;
	mBigArrowCursor = LoadCursor(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDC_CURSOR1));
	mDRM = nullptr;
	mShowHealthBar = false;
	mVoiceVolume = 0.0f;
	memset(&mFlowersPlucked, false, sizeof(mFlowersPlucked));
	mRIPMode = false;
	mPlayerLevelRef = -1;
	mEnableNativeHDR = false;
	mNativeHDRRenderer = false;
	mHDRPaperWhitePercent = 100;
	mHDRExposureTenthsEV = 0;
	mHDRAdaptiveToneMapping = false;
	mHDRToneMapTexture = nullptr;
	mHDRToneMapTextureScaleMilli = 0;
	mHDRToneMapUnavailable = false;
	mPreferredRefreshRateMilliHz = 0;
	mUseExclusiveFullscreen = false;
	mUseIntegerScaling = false;
}

//0x44EDD0、0x44EDF0
LawnApp::~LawnApp()
{
	DestroyHDRToneMapTexture();

	if (mBoard)
	{
		WriteCurrentUserConfig();
	}

	if (mBoard)
	{
		mBoardResult = BoardResult::BOARDRESULT_QUIT_APP;
		mBoard->TryToSaveGame();
		mWidgetManager->RemoveWidget(mBoard);
		delete mBoard;
		mBoard = nullptr;
	}

	if (mTitleScreen)
	{
		mWidgetManager->RemoveWidget(mTitleScreen);
		delete mTitleScreen;
	}

	delete mSoundSystem;
	delete mMusic;

	if (mKonamiCheck)
	{
		delete mKonamiCheck;
	}
	if (mMustacheCheck)
	{
		delete mMustacheCheck;
	}
	if (mMoustacheCheck)
	{
		delete mMoustacheCheck;
	}
	if (mSuperMowerCheck)
	{
		delete mSuperMowerCheck;
	}
	if (mSuperMowerCheck2)
	{
		delete mSuperMowerCheck2;
	}
	if (mFutureCheck)
	{
		delete mFutureCheck;
	}
	if (mPinataCheck)
	{
		delete mPinataCheck;
	}
	if (mDanceCheck)
	{
		delete mDanceCheck;
	}
	if (mDaisyCheck)
	{
		delete mDaisyCheck;
	}
	if (mSukhbirCheck)
	{
		delete mSukhbirCheck;
	}

	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		delete mGameSelector;
	}
	if (mChallengeScreen)
	{
		mWidgetManager->RemoveWidget(mChallengeScreen);
		delete mChallengeScreen;
	}
	if (mSeedChooserScreen)
	{
		mWidgetManager->RemoveWidget(mSeedChooserScreen);
		delete mSeedChooserScreen;
	}
	if (mAwardScreen)
	{
		mWidgetManager->RemoveWidget(mAwardScreen);
		delete mAwardScreen;
	}
	if (mCreditScreen)
	{
		mWidgetManager->RemoveWidget(mCreditScreen);
		delete mCreditScreen;
	}
	if (mLanguageScreen)
	{
		mWidgetManager->RemoveWidget(mLanguageScreen);
		delete mLanguageScreen;
	}
	if (mParticleScreen)
	{
		mWidgetManager->RemoveWidget(mParticleScreen);
		delete mParticleScreen;
	}

	delete mProfileMgr;
	delete mLastLevelStats;

	mResourceManager->DeleteResources("");
#ifdef _DEBUG
	BetaSubmit(true);
#endif
}

//0x44F200
void LawnApp::Shutdown()
{
	if (!mLoadingThreadCompleted)
	{
		mLoadingFailed = true;
		return;
	}

	if (!mShutdown)
	{
		for (int i = 0; i < Dialogs::NUM_DIALOGS; i++)
		{
			KillDialog(i);
		}

		if (mBoard)
		{
			mBoardResult = BoardResult::BOARDRESULT_QUIT_APP;
			mBoard->TryToSaveGame();
			KillBoard();
			WriteCurrentUserConfig();
		}

#ifdef _HAS_ZOMBATAR
		if (mGameSelector && mGameSelector->mZombatarWidget && mGameSelector->mZombatarWidget->mZombie) {
			mGameSelector->mZombatarWidget->mZombie->DieNoLoot();
			delete mGameSelector->mZombatarWidget->mZombie;
			mGameSelector->mZombatarWidget->mZombie = nullptr;
		}
#endif

		ProcessSafeDeleteList();

		if (mPoolEffect)
		{
			mPoolEffect->PoolEffectDispose();
			delete mPoolEffect;
			mPoolEffect = nullptr;
		}

		if (mZenGarden)
		{
			delete mZenGarden;
			mZenGarden = nullptr;
		}

		if (mEffectSystem)
		{
			mEffectSystem->EffectSystemDispose();
			delete mEffectSystem;
			mEffectSystem = nullptr;
		}

		if (mReanimatorCache)
		{
			mReanimatorCache->ReanimatorCacheDispose();
			delete mReanimatorCache;
			mReanimatorCache = nullptr;
		}

		DisposeZombatarClothesCache();
		FilterEffectDisposeForApp();
		TodParticleFreeDefinitions();
		ReanimatorFreeDefinitions();
		TrailFreeDefinitions();
		FreeGlobalAllocators();
		UpdateRegisterInfo();

		if (mPortAudioStream)
		{
			Pa_StopStream(mPortAudioStream);
			Pa_CloseStream(mPortAudioStream);
			Pa_Terminate();

			mPortAudioStream = nullptr;
		}

		SexyAppBase::Shutdown();

		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		if (mDRM)
		{
			delete mDRM;
		}
		mDRM = nullptr;
	}
}

//0x44F380
void LawnApp::KillBoard()
{
	if (mPortAudioStream)
	{
		Pa_StopStream(mPortAudioStream);
		Pa_CloseStream(mPortAudioStream);
		Pa_Terminate();

		mPortAudioStream = nullptr;
	}

	FinishModelessDialogs();
	KillSeedChooserScreen();
	if (mBoard)
	{
#ifdef _DEBUG
		BetaRecordLevelStats();
#endif
		mBoard->DisposeBoard();
		mWidgetManager->RemoveWidget(mBoard);
		SafeDeleteWidget(mBoard);
		mBoard = nullptr;
	}

	SetCursor(CURSOR_POINTER);
}

//0x44F410
bool LawnApp::CanPauseNow()
{
	if (mBoard == nullptr)  // 不在关卡内
		return false;

	if (mSeedChooserScreen && mSeedChooserScreen->mMouseVisible)  // 处于选卡界面
		return false;

	if (mBoard->mBoardFadeOutCounter >= 0)  // 退出关卡过程中
		return false;

	if (mCrazyDaveState != CrazyDaveState::CRAZY_DAVE_OFF)  // 存在戴夫
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)  // 处于禅境花园或智慧树
		return false;

	return GetDialogCount() <= 0;  // 不存在对话
}

void LawnApp::GotFocus()
{
}

//0x44F460
void LawnApp::LostFocus()
{
	if (!mTodCheatKeys && CanPauseNow())
	{
		DoPauseDialog();
	}
}

//0x44F480
void LawnApp::WriteToRegistry()
{
	if (mPlayerInfo)
	{
		RegistryWriteString("CurUser", SexyStringToString(mPlayerInfo->mName));
		mPlayerInfo->SaveDetails();
	}

	SexyAppBase::WriteToRegistry();
	RegistryWriteBoolean(_S("EnableNativeHDR"), mEnableNativeHDR);
	RegistryWriteInteger(_S("HDRPaperWhitePercent"), mHDRPaperWhitePercent);
	RegistryWriteInteger(_S("HDRExposureTenthsEV"), mHDRExposureTenthsEV);
	RegistryWriteBoolean(_S("HDRAdaptiveToneMapping"), mHDRAdaptiveToneMapping);
	RegistryWriteInteger(_S("PreferredRefreshRateMilliHz"), mPreferredRefreshRateMilliHz);
	RegistryWriteBoolean(_S("UseExclusiveFullscreen"), mUseExclusiveFullscreen);
	RegistryWriteBoolean(_S("UseIntegerScaling"), mUseIntegerScaling);
	RegistryWriteBoolean(_S("ShowFPS"), mShowFPS);
}

//0x44F530
void LawnApp::ReadFromRegistry()
{
	SexyApp::ReadFromRegistry();
	RegistryReadBoolean(_S("EnableNativeHDR"), &mEnableNativeHDR);
	RegistryReadInteger(_S("HDRPaperWhitePercent"), &mHDRPaperWhitePercent);
	RegistryReadInteger(_S("HDRExposureTenthsEV"), &mHDRExposureTenthsEV);
	RegistryReadBoolean(_S("HDRAdaptiveToneMapping"), &mHDRAdaptiveToneMapping);
	RegistryReadInteger(_S("PreferredRefreshRateMilliHz"), &mPreferredRefreshRateMilliHz);
	RegistryReadBoolean(_S("UseExclusiveFullscreen"), &mUseExclusiveFullscreen);
	RegistryReadBoolean(_S("UseIntegerScaling"), &mUseIntegerScaling);
	bool aShowFPS = mShowFPS;
	RegistryReadBoolean(_S("ShowFPS"), &aShowFPS);
	mHDRPaperWhitePercent = std::clamp(mHDRPaperWhitePercent, 50, 200);
	mHDRExposureTenthsEV = std::clamp(mHDRExposureTenthsEV, -20, 20);
	mPreferredRefreshRateMilliHz = std::clamp(mPreferredRefreshRateMilliHz, 0, 1000000);
	SetShowFPS(aShowFPS);
}

//0x44F540
bool LawnApp::WriteCurrentUserConfig()
{
	if (mPlayerInfo)
		mPlayerInfo->SaveDetails();

	return true;
}

//0x44F560
void LawnApp::PreNewGame(GameMode theGameMode, bool theLookForSavedGame)
{
	//if (NeedRegister())
	//{
	//	ShowGameSelector();
	//	return;
	//}

	mGameMode = theGameMode;
	if (theLookForSavedGame && TryLoadGame())
		return;

	SexyString aFileName = GetSavedGameName(mGameMode, mPlayerInfo->mId);
	EraseFile(aFileName);
	NewGame();
}

void LawnApp::PreNewGame(GameMode theGameMode, bool theLookForSavedGame, int theLevel)
{
	//if (NeedRegister())
	//{
	//	ShowGameSelector();
	//	return;
	//}

	mGameMode = theGameMode;
	if (theLookForSavedGame && TryLoadGame(theLevel))
		return;

	SexyString aFileName = GetSavedGameName(mGameMode, mPlayerInfo->mId, theLevel);
	EraseFile(aFileName);
	int thePlayerLevel = mPlayerInfo->mLevel;
	bool isReplaying = mPlayerLevelRef > theLevel;
	mPlayerLevelRef = isReplaying ? thePlayerLevel : -1;
	mPlayerInfo->mLevel = theLevel;
	NewGame();
	mBoard->mIsReplay = isReplaying;
	mPlayerInfo->mLevel = thePlayerLevel;
}

//0x44F5F0
void LawnApp::MakeNewBoard()
{
	KillBoard();
	mBoard = new Board(this);
	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);
	mWidgetManager->BringToBack(mBoard);
	mWidgetManager->SetFocus(mBoard);
	if (!mPortAudioStream && ChallengeUsesMicrophone(mGameMode))
	{
		if (!TryToInitializePA())
		{
			DoDialog(Dialogs::DIALOG_INFO, true, "Error", "Something went wrong when starting Port Audio.\n\n Please check your Audio driver or whether your system meets the requirements to support PortAudio. Please contact the devoleper", _S("[OK_LABEL]"), Dialog::BUTTONS_FOOTER);
			mPortAudioStream = nullptr;
			DoBackToMain();
			return;
		}
	}
}

//0x44F6B0
void LawnApp::StartPlaying()
{
	KillSeedChooserScreen();
	mBoard->StartLevel();
	mGameScene = GameScenes::SCENE_PLAYING;
}

//0x44F700
bool LawnApp::SaveFileExists()
{
	SexyString aFileName = GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mPlayerInfo->mId);
	return this->FileExists(aFileName);
}

//0x44F7A0
bool LawnApp::TryLoadGame()
{
	return TryLoadGame(mPlayerInfo->mLevel);
}

bool LawnApp::TryLoadGame(int theLevel)
{
	SexyString aSaveName = GetSavedGameName(mGameMode, mPlayerInfo->mId, theLevel);
	mMusic->StopAllMusic();

	if (this->FileExists(aSaveName))
	{
		MakeNewBoard();
		if (mBoard->LoadGame(aSaveName))
		{
			mFirstTimeGameSelector = false;
			DoContinueDialog();
			return true;
		}

		KillBoard();
	}

	return false;
}


//0x44F890
void LawnApp::NewGame()
{
	mFirstTimeGameSelector = false;

	MakeNewBoard();
	if (!mBoard) return;
	mBoard->InitLevel();
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mGameScene = GameScenes::SCENE_LEVEL_INTRO;

	ShowSeedChooserScreen();
	mBoard->mCutScene->StartLevelIntro();
}

//0x44F8E0
void LawnApp::ShowGameSelector()
{
	KillBoard();
	//UpdateRegisterInfo();
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
	}
	mGameMode = GameMode::GAMEMODE_ADVENTURE;
	mGameScene = GameScenes::SCENE_MENU;
	mGameSelector = new GameSelector(this);
	mGameSelector->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mGameSelector);
	mWidgetManager->BringToBack(mGameSelector);
	mWidgetManager->SetFocus(mGameSelector);

	//if (NeedRegister())
	//{
	//	DoNeedRegisterDialog();
	//}
}

//0x44F9E0
void LawnApp::KillGameSelector()
{
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
		mGameSelector = nullptr;
	}
}

//0x44FA20
void LawnApp::ShowAwardScreen(AwardType theAwardType, bool theShowAchievements)
{
	mGameScene = GameScenes::SCENE_AWARD;
	mAwardScreen = new AwardScreen(this, theAwardType, theShowAchievements);
	mAwardScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mAwardScreen);
	mWidgetManager->BringToBack(mAwardScreen);
	mWidgetManager->SetFocus(mAwardScreen);
}

//0x44FAF0
void LawnApp::KillAwardScreen()
{
	if (mAwardScreen)
	{
		mWidgetManager->RemoveWidget(mAwardScreen);
		SafeDeleteWidget(mAwardScreen);
		mAwardScreen = nullptr;
	}
}

//0x44FB30
void LawnApp::ShowCreditScreen()
{
	mCreditScreen = new CreditScreen(this);
	mCreditScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mCreditScreen);
	mWidgetManager->BringToBack(mCreditScreen);
	mWidgetManager->SetFocus(mCreditScreen);
}

//0x44FBF0
void LawnApp::KillCreditScreen()
{
	if (mCreditScreen)
	{
		mWidgetManager->RemoveWidget(mCreditScreen);
		SafeDeleteWidget(mCreditScreen);
		mCreditScreen = nullptr;
	}
}

//0x44FC30
void LawnApp::ShowChallengeScreen(ChallengePage thePage)
{
	mGameScene = GameScenes::SCENE_CHALLENGE;
	mChallengeScreen = new ChallengeScreen(this, thePage);
	mChallengeScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mChallengeScreen);
	mWidgetManager->BringToBack(mChallengeScreen);
	mWidgetManager->SetFocus(mChallengeScreen);
}

//0x44FD00
void LawnApp::KillChallengeScreen()
{
	if (mChallengeScreen)
	{
		mWidgetManager->RemoveWidget(mChallengeScreen);
		SafeDeleteWidget(mChallengeScreen);
		mChallengeScreen = nullptr;
	}
}

//0x44FD40
StoreScreen* LawnApp::ShowStoreScreen()
{
	//FinishModelessDialogs();
	TOD_ASSERT(!GetDialog((int)Dialogs::DIALOG_STORE));

	StoreScreen* aStoreScreen = new StoreScreen(this);
	AddDialog(aStoreScreen);
	mWidgetManager->SetFocus(aStoreScreen);

	return aStoreScreen;
}

void LawnApp::KillStoreScreen()
{
	if (GetDialog(Dialogs::DIALOG_STORE))
	{
		KillDialog(Dialogs::DIALOG_STORE);
		ClearUpdateBacklog(false);
	}
}

//0x44FDC0
void LawnApp::ShowSeedChooserScreen()
{
	TOD_ASSERT(mSeedChooserScreen == nullptr);

	mSeedChooserScreen = new SeedChooserScreen();
	mSeedChooserScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mSeedChooserScreen);
	mWidgetManager->BringToBack(mSeedChooserScreen);
}

//0x44FE70
void LawnApp::KillSeedChooserScreen()
{
	if (mSeedChooserScreen)
	{
		mWidgetManager->RemoveWidget(mSeedChooserScreen);
		SafeDeleteWidget(mSeedChooserScreen);
		mSeedChooserScreen = nullptr;
	}
}

void LawnApp::EndLevel()
{
	KillBoard();
	if (IsAdventureMode())
	{
		NewGame();
	}

	mFirstTimeGameSelector = true;

	MakeNewBoard();
	mBoard->InitLevel();
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mGameScene = GameScenes::SCENE_LEVEL_INTRO;
	ShowSeedChooserScreen();
	mBoard->mCutScene->StartLevelIntro();
}

//0x44FEB0
void LawnApp::DoBackToMain()
{
	mMusic->StopAllMusic();
	mSoundSystem->CancelPausedFoley();
	WriteCurrentUserConfig();
	KillNewOptionsDialog();
	KillBoard();
	ShowGameSelector();
}

//0x44FF00
void LawnApp::DoConfirmBackToMain()
{
	LawnDialog* aDialog = (LawnDialog*)DoDialog(
		Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN, 
		true, 
		_S("Leave Game?"/*"[LEAVE_GAME]"*/),
		_S("Do you want to return\nto the main menu?\n\nYour game will be saved."/*"[LEAVE_GAME_HEADER]"*/), 
		_S(""), 
		Dialog::BUTTONS_YES_NO
	);

	aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[LEAVE_BUTTON]")); 
	aDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_CANCEL]"));
	//aDialog->CalcSize(0, 0);
}

//0x4500D0
void LawnApp::DoNewOptions(bool theFromGameSelector)
{
	//FinishModelessDialogs();

	NewOptionsDialog* aDialog = new NewOptionsDialog(this, theFromGameSelector);
	CenterDialog(aDialog, IMAGE_OPTIONS_MENUBACK->mWidth, IMAGE_OPTIONS_MENUBACK->mHeight);
	AddDialog(Dialogs::DIALOG_NEWOPTIONS, aDialog);
	mWidgetManager->SetFocus(aDialog);
}

//0x450180
AlmanacDialog* LawnApp::DoAlmanacDialog(SeedType theSeedType, ZombieType theZombieType)
{
	PerfTimer mTimer;
	mTimer.Start();

	//FinishModelessDialogs();

	AlmanacDialog* aDialog = new AlmanacDialog(this);
	AddDialog(Dialogs::DIALOG_ALMANAC, aDialog);
	mWidgetManager->SetFocus(aDialog);

	if (theSeedType != SeedType::SEED_NONE)
	{
		aDialog->ShowPlant(theSeedType);
	}
	else if (theZombieType != ZombieType::ZOMBIE_INVALID)
	{
		aDialog->ShowZombie(theZombieType);
	}

	int aDuration = mTimer.GetDuration();
	TodTrace("almanac load time: %d ms", aDuration);

	return aDialog;
}

//0x450220
void LawnApp::DoContinueDialog()
{
	ContinueDialog* aDialog = new ContinueDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CONTINUE, aDialog);
}

//0x4502C0
void LawnApp::DoPauseDialog()
{
	mBoard->Pause(true);
	//FinishModelessDialogs();

	LawnDialog* aDialog = (LawnDialog*)DoDialog(
		Dialogs::DIALOG_PAUSED,
		true,
		_S("GAME PAUSED"/*"[GAME_PAUSED]"*/),
		_S("Click to resume game"), 
		_S("Resume Game"/*"[RESUME_GAME]"*/),
		Dialog::BUTTONS_FOOTER
	);

	aDialog->mReanimation->AddReanimation(72.0f, 42.0f, ReanimationType::REANIM_ZOMBIE_NEWSPAPER);
	aDialog->mSpaceAfterHeader = 155;
	aDialog->CalcSize(0, 10);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
}

//0x4504B0
int LawnApp::LawnMessageBox(int theDialogId, const SexyChar* theHeaderName, const SexyChar* theLinesName, const SexyChar* theButton1Name, const SexyChar* theButton2Name, int theButtonMode)
{
	Widget* aOldFocus = mWidgetManager->mFocusWidget;

	LawnDialog* aDialog = (LawnDialog*)DoDialog(theDialogId, true, theHeaderName, theLinesName, theButton1Name, theButtonMode);
	if (aDialog->mYesButton)
	{
		aDialog->mYesButton->mLabel = TodStringTranslate(theButton1Name);
	}
	if (aDialog->mNoButton)
	{
		aDialog->mNoButton->mLabel = TodStringTranslate(theButton2Name);
	}
	//aDialog->CalcSize(0, 0);

	mWidgetManager->SetFocus(aDialog);
	int aResult = aDialog->WaitForResult(true);
	mWidgetManager->SetFocus(aOldFocus);

	return aResult;
}

//0x450770
Dialog* LawnApp::DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	SexyString aHeader = TodStringTranslate(theDialogHeader);
	SexyString aLines = TodStringTranslate(theDialogLines);
	SexyString aFooter = TodStringTranslate(theDialogFooter);

	Dialog* aDialog = SexyAppBase::DoDialog(theDialogId, isModal, aHeader, aLines, aFooter, theButtonMode);
	if (mWidgetManager->mFocusWidget == nullptr)
	{
		mWidgetManager->mFocusWidget = aDialog;
	}

	return aDialog;
}

Dialog* LawnApp::DoDialogDelay(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	LawnDialog* aDialog = (LawnDialog*)SexyAppBase::DoDialog(theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);
	aDialog->SetButtonDelay(30);
	return aDialog;
}

//0x450880
void LawnApp::DoUserDialog()
{
	KillDialog(Dialogs::DIALOG_USERDIALOG);

	UserDialog* aDialog = new UserDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_USERDIALOG, aDialog);
	mWidgetManager->SetFocus(aDialog);
}

//0x450930
void LawnApp::FinishUserDialog(bool isYes)
{
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (aUserDialog)
	{
		if (isYes)
		{
			PlayerInfo* aProfile = mProfileMgr->GetProfile(aUserDialog->GetSelName());
			if (aProfile)
			{
				mPlayerInfo = aProfile;
				mWidgetManager->MarkAllDirty();

				if (mGameSelector)
				{
					mGameSelector->SyncProfile(true);
				}
			}
			mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
		}

		KillDialog(Dialogs::DIALOG_USERDIALOG);
	}
}

//0x450A10
void LawnApp::DoCreateUserDialog()
{
	KillDialog(Dialogs::DIALOG_CREATEUSER);

	NewUserDialog* aDialog = new NewUserDialog(this, false);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CREATEUSER, aDialog);
}

//0x450AC0
void LawnApp::FinishCreateUserDialog(bool isYes)
{
	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(Dialogs::DIALOG_CREATEUSER);
	if (aNewUserDialog == nullptr)
		return;

	SexyString aName = aNewUserDialog->GetName();

	if (isYes && aName.empty())
	{
		DoDialog(
			Dialogs::DIALOG_CREATEUSERERROR,
			true,
			_S("Enter Your Name"),
			_S("Please enter your name to create a new user profile for storing high score data and game progress"),
			_S("OK"),
			Dialog::BUTTONS_FOOTER
		);
	}
	else if (mPlayerInfo == nullptr && (!isYes || aName.empty()))
	{
		DoDialog(
			Dialogs::DIALOG_CREATEUSERERROR,
			true,
			_S("Enter Your Name"/*"[ENTER_YOUR_NAME]"*/),
			_S("Please enter your name to create a new user profile for storing high score data and game progress"/*"[ENTER_NEW_USER]"*/),
			_S("OK"/*"[DIALOG_BUTTON_OK]"*/),
			Dialog::BUTTONS_FOOTER
		);
	}
	else if (!isYes)
	{
		KillDialog(Dialogs::DIALOG_CREATEUSER);
	}
	else
	{
		PlayerInfo* aProfile = mProfileMgr->AddProfile(aName);
		if (aProfile == nullptr)
		{
			DoDialog(
				Dialogs::DIALOG_CREATEUSERERROR,
				true,
				_S("Name Conflict"/*"[NAME_CONFLICT]"*/),
				_S("The name you entered is already being used.  Please enter a unique player name"/*"[ENTER_UNIQUE_PLAYER_NAME]"*/),
				_S("OK"/*"[DIALOG_BUTTON_OK]"*/),
				Dialog::BUTTONS_FOOTER
			);
		}
		else
		{
			mProfileMgr->Save();
			mPlayerInfo = aProfile;

			/*string name = mPlayerInfo->mName;

			transform(name.begin(), name.end(), name.begin(), ::tolower);
			
			if (name == "patrice")
			{
				mPlayerInfo->mHardMode = true;
				LawnDialog* aDialog = (LawnDialog*)DoDialog(
					Dialogs::DIALOG_PAUSED,
					true,
					_S("WARNING!"),
					_S("You are now playing HARD MODE!"),
					_S("Continue"),
					Dialog::BUTTONS_FOOTER
				);

				aDialog->mReanimation->AddReanimation(72.0f, 67.0f, ReanimationType::REANIM_GARGANTUAR);
				aDialog->mReanimation->mReanim->SetImageOverride("anim_head1", IMAGE_REANIM_ZOMBIE_GARGANTUAR_HEAD_REDEYE);
				aDialog->mSpaceAfterHeader = 225;
				aDialog->CalcSize(0, 10);
				CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
			}*/

			KillDialog(Dialogs::DIALOG_USERDIALOG);
			KillDialog(Dialogs::DIALOG_CREATEUSER);
			mWidgetManager->MarkAllDirty();

			if (mGameSelector)
			{
				mGameSelector->SyncProfile(true);
			}
		}
	}
}

//0x450E20
void LawnApp::DoConfirmDeleteUserDialog(const SexyString& theName)
{
	KillDialog(Dialogs::DIALOG_CONFIRMDELETEUSER);
	DoDialog(
		Dialogs::DIALOG_CONFIRMDELETEUSER, 
		true, 
		_S("Are You Sure"/*"[ARE_YOU_SURE]"*/), 
		// StrFormat(TodStringTranslate(_S("[DELETE_USER_WARNING]")).c_str(), StringToSexyStringFast(theName))
		StrFormat(_S("This will permanently remove '%s' from the player roster!"), theName.c_str()),
		_S(""), 
		Dialog::BUTTONS_YES_NO
	);
}

//0x450F40
void LawnApp::FinishConfirmDeleteUserDialog(bool isYes)
{
	KillDialog(Dialogs::DIALOG_CONFIRMDELETEUSER);
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (aUserDialog == nullptr)
		return;

	mWidgetManager->SetFocus(aUserDialog);

	if (!isYes)
		return;

	SexyString aCurName = mPlayerInfo ? mPlayerInfo->mName : _S("");
	SexyString aName = aUserDialog->GetSelName();
	if (aName == aCurName)
	{
		mPlayerInfo = nullptr;
	}

	mProfileMgr->DeleteProfile(aName);
	aUserDialog->FinishDeleteUser();
	if (mPlayerInfo == nullptr)
	{
		mPlayerInfo = mProfileMgr->GetProfile(aUserDialog->GetSelName());
		if (mPlayerInfo == nullptr)
		{
			mPlayerInfo = mProfileMgr->GetAnyProfile();
		}
		mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
	}

	mProfileMgr->Save();
	if (mPlayerInfo == nullptr)
	{
		DoCreateUserDialog();
	}

	mWidgetManager->MarkAllDirty();
	if (mGameSelector != nullptr)
	{
		mGameSelector->SyncProfile(true);
	}
}

//0x451180
void LawnApp::DoRenameUserDialog(const SexyString& theName)
{
	KillDialog(Dialogs::DIALOG_RENAMEUSER);

	NewUserDialog* aDialog = new NewUserDialog(this, true);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	aDialog->SetName(theName);
	AddDialog(Dialogs::DIALOG_RENAMEUSER, aDialog);
}

//0x451260
void LawnApp::FinishRenameUserDialog(bool isYes)
{
	UserDialog* aUserDialog = (UserDialog*)GetDialog(Dialogs::DIALOG_USERDIALOG);
	if (!isYes)
	{
		KillDialog(Dialogs::DIALOG_RENAMEUSER);
		mWidgetManager->SetFocus(aUserDialog);
		return;
	}

	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(Dialogs::DIALOG_RENAMEUSER);
	if (aUserDialog == nullptr || aNewUserDialog == nullptr)
		return;

	SexyString anOldName = aUserDialog->GetSelName();
	SexyString aNewName = aNewUserDialog->GetName();
	if (aNewName.empty())
		return;
	
	bool isCurrentUser = mProfileMgr->GetProfile(anOldName) == mPlayerInfo;
	if (!mProfileMgr->RenameProfile(anOldName, aNewName))
	{
		DoDialog(
			Dialogs::DIALOG_RENAMEUSERERROR,
			true,
			_S("Name Conflict"/*"[NAME_CONFLICT]"*/),
			_S("The name you entered is already being used.  Please enter a unique player name"/*"[ENTER_UNIQUE_PLAYER_NAME]"*/),
			_S("OK"/*"[DIALOG_BUTTON_OK]"*/),
			Dialog::BUTTONS_FOOTER
		);
		return;
	}

	mProfileMgr->Save();
	if (isCurrentUser)
	{
		mPlayerInfo = mProfileMgr->GetProfile(aNewName);
	}
	mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;

	aUserDialog->FinishRenameUser(aNewName);
	mWidgetManager->MarkAllDirty();
	KillDialog(Dialogs::DIALOG_RENAMEUSER);
	mWidgetManager->SetFocus(aUserDialog);
}

//0x451490
void LawnApp::FinishNameError(int theId)
{
	KillDialog(theId);

	NewUserDialog* aNewUserDialog = (NewUserDialog*)GetDialog(theId == Dialogs::DIALOG_CREATEUSERERROR ? Dialogs::DIALOG_CREATEUSER : Dialogs::DIALOG_RENAMEUSER);
	if (aNewUserDialog)
	{
		mWidgetManager->SetFocus(aNewUserDialog->mNameEditWidget);
	}
}

//0x4514D0
void LawnApp::FinishRestartConfirmDialog()
{
	mSawYeti = mBoard->mKilledYeti;
	int theLevel = -1;

	KillDialog(Dialogs::DIALOG_CONTINUE);
	KillDialog(Dialogs::DIALOG_RESTARTCONFIRM);
	if (mBoard && mGameMode == GameMode::GAMEMODE_ADVENTURE)
		theLevel = mBoard->mLevel;
	KillBoard();

	if (theLevel > 0)
		PreNewGame(mGameMode, false, theLevel);
	else
		PreNewGame(mGameMode, false);
}

void LawnApp::DoCheatDialog()
{
	KillDialog(Dialogs::DIALOG_CHEAT);

	CheatDialog* aDialog = new CheatDialog(this);
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	AddDialog(Dialogs::DIALOG_CHEAT, aDialog);
}

void LawnApp::FinishCheatDialog(bool isYes)
{
	CheatDialog* aCheatDialog = (CheatDialog*)GetDialog(Dialogs::DIALOG_CHEAT);
	if (aCheatDialog == nullptr)
		return;

	if (isYes && !aCheatDialog->ApplyCheat())
		return;

	KillDialog(Dialogs::DIALOG_CHEAT);
	if (isYes)
	{
		mMusic->StopAllMusic();
		mBoardResult = BoardResult::BOARDRESULT_CHEAT;
		PreNewGame(mGameMode, false);
	}
}

void LawnApp::FinishTimesUpDialog()
{
	KillDialog(Dialogs::DIALOG_TIMESUP);
}

void LawnApp::DoConfirmSellDialog(const SexyString& theMessage)
{
	Dialog* aConfirmDialog = DoDialog(Dialogs::DIALOG_ZEN_SELL, true, _S("[ZEN_SELL_HEADER]"), theMessage, _S(""), Dialog::BUTTONS_YES_NO);
	aConfirmDialog->mYesButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_YES]"));
	aConfirmDialog->mNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_NO]"));
}

void LawnApp::DoConfirmPurchaseDialog(const SexyString& theMessage)
{
	LawnDialog* aComfirmDialog = (LawnDialog*)DoDialog(Dialogs::DIALOG_STORE_PURCHASE, true, _S("买下这个物品？"), theMessage, _S(""), Dialog::BUTTONS_YES_NO);
	aComfirmDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_YES]"));
	aComfirmDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_NO]"));
}

//0x451580
Dialog* LawnApp::NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	LawnDialog* aDialog = new LawnDialog(
		this, 
		theDialogId, 
		isModal, 
		theDialogHeader, 
		theDialogLines, 
		theDialogFooter, 
		theButtonMode
	);

	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	return aDialog;
}

//0x451630
bool LawnApp::KillNewOptionsDialog()
{
	NewOptionsDialog* aNewOptionsDialog = (NewOptionsDialog*)GetDialog(Dialogs::DIALOG_NEWOPTIONS);
	if (aNewOptionsDialog == nullptr)
		return false;

	bool wantWindowed = !aNewOptionsDialog->mFullscreenCheckbox->IsChecked();
	//bool want3D = aNewOptionsDialog->mHardwareAccelerationCheckbox->IsChecked();
	mEnableVsync = aNewOptionsDialog->mHardwareAccelerationCheckbox->IsChecked();
	RegistryWriteBoolean(_S("EnableVsync"), mEnableVsync);
	SDL_SetRenderVSync(mSDLRenderer, mEnableVsync);
	mEnableNativeHDR = aNewOptionsDialog->mNativeHDRCheckbox->IsChecked();
	RegistryWriteBoolean(_S("EnableNativeHDR"), mEnableNativeHDR);
	SwitchScreenMode(wantWindowed, true, false);

	KillDialog(Dialogs::DIALOG_NEWOPTIONS);
	ClearUpdateBacklog();
	return true;
}

//0x4516C0
bool LawnApp::KillAlmanacDialog()
{
	if (GetDialog(Dialogs::DIALOG_ALMANAC))
	{
		KillDialog(Dialogs::DIALOG_ALMANAC);
		ClearUpdateBacklog(false);
		return true;
	}

	return false;
}

//0x4516F0
bool LawnApp::NeedPauseGame()
{
	if (mDialogList.size() == 0)
		return false;

	if (mDialogList.size() == 1 && mDialogList.front()->mId != Dialogs::DIALOG_NEW_GAME)
	{
		int anId = mDialogList.front()->mId;
		if (anId == Dialogs::DIALOG_CHOOSER_WARNING || anId == Dialogs::DIALOG_PURCHASE_PACKET_SLOT || anId == Dialogs::DIALOG_IMITATER)
		{
			return false;
		}
	}

	return (mBoard == nullptr || mGameMode != GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN) && (mBoard == nullptr || mGameMode != GameMode::GAMEMODE_TREE_OF_WISDOM);
}

//0x451780
void LawnApp::ModalOpen()
{
	if (mBoard && NeedPauseGame())
	{
		mBoard->Pause(true);
	}
}

void LawnApp::ModalClose()
{
}

//0x451800
bool LawnApp::KillDialog(int theDialogId)
{
	if (SexyAppBase::KillDialog(theDialogId))
	{
		if (mDialogMap.size() == 0)
		{
			if (mBoard)
			{
				mWidgetManager->SetFocus(mBoard);
			}
			else if (mGameSelector)
			{
				mWidgetManager->SetFocus(mGameSelector);
			}
		}

		if (mBoard && !NeedPauseGame())
		{
			mBoard->Pause(false);
		}

		return true;
	}

	return false;
}

//0x451870
void LawnApp::ShowResourceError(bool doExit)
{
	SexyAppBase::ShowResourceError(doExit);
}

void BetaSubmitFunc()
{
	if (gLawnApp)
	{
		gLawnApp->BetaSubmit(false);
	}
}

SDL_Cursor* CreateCursorFromResource(HINSTANCE hInstance, int resourceID, int hotX, int hotY)
{
	HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resourceID), RT_RCDATA);
	HGLOBAL hResData = LoadResource(hInstance, hRes);
	void* pData = LockResource(hResData);
	DWORD size = SizeofResource(hInstance, hRes);
	SDL_IOStream* io = SDL_IOFromConstMem(pData, size);
	SDL_Surface* surface = SDL_LoadBMP_IO(io, true);
	SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotX, hotY);
	SDL_DestroySurface(surface);
	return cursor;
}

//HotSpot: 11 4
//Size: 32 32
unsigned char mFingerCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3,
	0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff,
	0xc0, 0x07, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xfc, 0x40, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff,
	0xfc, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0,
	0x03, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
	0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x18, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1b, 0x60, 0x00, 0x00, 0x1b, 0x68, 0x00,
	0x00, 0x1b, 0x6c, 0x00, 0x01, 0x9f, 0xec, 0x00, 0x01, 0xdf, 0xfc, 0x00, 0x00, 0xdf, 0xfc,
	0x00, 0x00, 0x5f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f,
	0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
	0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

//HotSpot: 15 10
//Size: 32 32
unsigned char mDraggingCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xe0,
	0x01, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff,
	0xe0, 0x00, 0xff, 0xfe, 0x60, 0x00, 0xff, 0xfc, 0x20, 0x00, 0xff, 0xfc, 0x00, 0x00, 0xff,
	0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0,
	0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x80, 0x00, 0x00, 0x01, 0xb0, 0x00, 0x00, 0x0d, 0xb0, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00,
	0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00,
	0x01, 0x8d, 0xb6, 0x00, 0x01, 0xcf, 0xfe, 0x00, 0x00, 0xef, 0xfe, 0x00, 0x00, 0xff, 0xfe,
	0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x1f,
	0xfc, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00,
	0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

SDL_Cursor* CreateCursorFromRaw(const unsigned char* src, int w, int h, int hotX, int hotY)
{
	size_t maskSize = (w * h) / 8;
	SDL_Surface* surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB8888);

	Uint32* pixels = (Uint32*)surface->pixels;
	int pitch = surface->pitch / sizeof(Uint32);  
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			pixels[y * pitch + x] = 0x00000000;  
		}
	}
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int byteIndex = y * (w / 8) + (x / 8);
			int bitIndex = 7 - (x % 8); 

			Uint8 andBit = (src[byteIndex] >> bitIndex) & 1;
			Uint8 xorBit = (src[byteIndex + maskSize] >> bitIndex) & 1;

			if (andBit == 0) {  
				if (xorBit == 0) {
					pixels[y * pitch + x] = 0xFF000000;
				}
				else {
					pixels[y * pitch + x] = 0xFFFFFFFF;  
				}
			}
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotX, hotY);
	SDL_DestroySurface(surface);
	return cursor;
}

//0x451880
void LawnApp::Init()
{
	DoParseCmdLine();
	if (!mTodCheatKeys)
	{
		mOnlyAllowOneCopyToRun = false;
	}

	//if (!gSexyCache->Connected() &&
	//	gLawnApp->mTodCheatKeys &&
	//	MessageBox(gLawnApp->mHWnd, _S("Start SexyCache now?"), _S("SexyCache"), MB_YESNO) == IDYES &&
	//	WinExec("SexyCache.exe", SW_MINIMIZE) >= 32)
	//{
	//	gSexyCache = SexyCache();
	//}
	//if (gSexyCache->Connected() && !gLawnApp->mTodCheatKeys)
	//{
	//	gSexyCache->Disconnect();
	//}

	mSessionID = _time32(nullptr);
	mPlayTimeActiveSession = 0;
	mPlayTimeInactiveSession = 0;
	mBoardResult = BoardResult::BOARDRESULT_NONE;
	mSawYeti = false;

	SexyApp::Init();

#ifdef _DEBUG
	TodAssertInitForApp();
	gBetaSubmitFunc = BetaSubmitFunc;
	TodLog("session id: %u", mSessionID);
#endif

	
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	bool IsXMLPackLoaded = mResourceManager->ParseResourcesFile(_S("properties\\resources.xml"));

	if (GetFileAttributesExA("dependency\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("dependency\\properties\\resources.xml"))
	{
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("dependency\\properties\\resources.xml"));
	}
	else
	{
		MsgBox("dependency files missing! Please get the dependency.pak from the Stable Decompile Github Repo. Contact the developers.", "Error");
	}

	if (GetFileAttributesExA("extension\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("extension\\properties\\resources.xml"))
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("extension\\properties\\resources.xml"));

#ifdef _ALLOW_RESOURCE_PACKS
	if (GetFileAttributesExA("resourcepack\\properties\\resources.xml", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("resourcepack\\properties\\resources.xml"))
		IsXMLPackLoaded &= mResourceManager->ParseResourcesFile(_S("resourcepack\\properties\\resources.xml"));
#endif

	if (!IsXMLPackLoaded)
	{
		ShowResourceError(true);
		return;
	}

	if (!TodLoadResources("Init"))
	{
		return;
	}

	PerfTimer mTimer;
	mTimer.Start();

	mProfileMgr->Load();

	std::string theUser;
	SexyString aCurUser;
	if (mPlayerInfo == nullptr && RegistryReadString("CurUser", &theUser))
	{
		aCurUser = StringToSexyString(theUser);
		mPlayerInfo = mProfileMgr->GetProfile(aCurUser);
		mPlayerLevelRef = mPlayerInfo ? mPlayerInfo->GetLevel() : -1;
	}
	if (mPlayerInfo == nullptr)
	{
		mPlayerInfo = mProfileMgr->GetAnyProfile();
	}

	mMaxExecutions = GetInteger("MaxExecutions", 0);
	mMaxPlays = GetInteger("MaxPlays", 0);
	mMaxTime = GetInteger("MaxTime", 60);

	mTitleScreen = new TitleScreen(this);
	mTitleScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mTitleScreen);
	mWidgetManager->SetFocus(mTitleScreen);
	
#ifdef _DEBUG
	int aDuration = mTimer.GetDuration();
	TodTrace("loading: 'profiles' %d ms", aDuration);
#endif
	mTimer.Start();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	TTF_Init();
	HINSTANCE hInstance = GetModuleHandle(NULL);
	mSDLPointerCursor = CreateCursorFromResource(hInstance, IDC_CURSOR1, 0, 0);
	mSDLHandCursor = CreateCursorFromRaw(mFingerCursorData, 32, 32, 11, 4);
	mSDLDraggingCursor = CreateCursorFromRaw(mDraggingCursorData, 32, 32, 15, 10);
	mSDLTextCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
	mSDLWaitCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	mSDLNoCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);

	mMusic = new Music();
	mSoundSystem = new TodFoley();
	mEffectSystem = new EffectSystem();
	mEffectSystem->EffectSystemInitialize();

	mKonamiCheck = new TypingCheck();
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_UP);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_UP);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_DOWN);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_DOWN);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_LEFT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_RIGHT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_LEFT);
	mKonamiCheck->AddKeyCode(KeyCode::KEYCODE_RIGHT);
	mKonamiCheck->AddChar('b');
	mKonamiCheck->AddChar('a');
	mMustacheCheck = new TypingCheck("mustache");
	mMoustacheCheck = new TypingCheck("moustache");
	mSuperMowerCheck = new TypingCheck("trickedout");
	mSuperMowerCheck2 = new TypingCheck("tricked out");
	mFutureCheck = new TypingCheck("future");
	mPinataCheck = new TypingCheck("pinata");
	mDanceCheck = new TypingCheck("dance");
	mDaisyCheck = new TypingCheck("daisies");
	mSukhbirCheck = new TypingCheck("sukhbir");

#ifdef _DEBUG
	aDuration = mTimer.GetDuration();
	TodTrace("loading: 'system' %d ms", aDuration);
#endif
	mTimer.Start();

	ReanimatorLoadDefinitions(gLawnReanimationArray, ReanimationType::NUM_REANIMS);
	ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LOADBAR_SPROUT, true);
	ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LOADBAR_ZOMBIEHEAD, true);

#ifdef _DEBUG
	aDuration = mTimer.GetDuration();
	TodTrace("loading: 'loaderbar' %d ms", aDuration);
#endif
	mTimer.Start();

	SDL_RaiseWindow(LawnApp::mSDLWindow);
	if (!IsScreenSaver() && !IsParticleEditor())
		PlayVideo(StrFormat("%svideos/intro.mp4", SDL_GetBasePath()).c_str(), true, Color::Black);
}

//0x4522A0
bool LawnApp::ChangeDirHook(const char* theIntendedPath)
{
	return false;
}

//0x4522B0
void LawnApp::Start()
{
	if (mLoadingFailed)
		return;

	SexyAppBase::Start();
}

int LawnApp::AudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	if (!inputBuffer) {
		*(float*)userData = 0.0f;
		return paContinue;
	}

	const float* input = (const float*)inputBuffer;
	float rms = 0.0f;
	for (unsigned long i = 0; i < framesPerBuffer; ++i) {
		rms += input[i] * input[i];  // 
	}

	rms = sqrt(rms / framesPerBuffer);

	float* volume = (float*)userData;
	*volume = rms;

	return paContinue;
}
//0x4522C0
bool LawnApp::DebugKeyDown(int theKey)
{
	return SexyAppBase::DebugKeyDown(theKey);
}

//0x4522E0
void LawnApp::HandleCmdLineParam(const std::string& theParamName, const std::string& theParamValue)
{
	if (theParamName == "-tod")
	{
#ifdef _DEBUG
		mTodCheatKeys = true;
		mDebugKeysEnabled = true;
#endif
	}
	else
	{
		SexyApp::HandleCmdLineParam(theParamName, theParamValue);
	}
}

//0x452310
bool LawnApp::UpdatePlayerProfileForFinishingLevel()
{
	bool aUnlockedNewChallenge = false;

	if (IsAdventureMode())
	{
		if (abs(mBoard->mLevel) == FINAL_LEVEL)
		{
			if (mPlayerInfo->mFinishedAdventure == 1)
			{
				if (mBoard->mLevel >= 0)
				{
					mPlayerInfo->SetLevel(51);
					mPlayerInfo->mFinishedAdventure++;
				}
				else
				{
					mPlayerInfo->SetLevel(-49);
				}
			}
			else
			{
				if (mBoard->mLevel >= 0)
				{
					mPlayerInfo->SetLevel(1);  // 存档回到第 1-1 关
					mPlayerInfo->mFinishedAdventure++;
				}
				else
				{
					mPlayerInfo->SetLevel(-49);
				}
			}

			  // 完成冒险模式周目数增加 1 次
			if (mPlayerInfo->mFinishedAdventure == 1)
			{
				mPlayerInfo->mNeedsMessageOnGameSelector = 1;
			}
			ReportAchievement::GiveAchievement(this, AchievementId::HomeSecurity, false); // @Patoke: add achievement
		}
		else
		{
			if (!mBoard->mIsReplay)
				mPlayerInfo->SetLevel(mBoard->mLevel + 1);  // 存档进入下一关
		}

		if (!HasFinishedAdventure() && abs(mPlayerInfo->mLevel) == 34)
		{
			mPlayerInfo->mNeedsMagicTacoReward = 1;
		}

		// @Patoke: implemented
		if (mBoard->StageIsDayWithPool() && !mBoard->mPeaShooterUsed) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::DontPea, false);
		}
		if (mBoard->StageHasRoof() && !mBoard->HasConveyorBeltSeedBank() && !mBoard->mCatapultPlantsUsed) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::Grounded, false);
		}
		if (mBoard->StageIsNight() && !mBoard->mMushroomsUsed && mPlayerInfo->mLevel != 35 && mPlayerInfo->mLevel != 40)
		{
			ReportAchievement::GiveAchievement(this, AchievementId::NoFungusAmongUs, false);
		}
		if (!mBoard->StageIsNight() && mBoard->mMushroomAndCoffeeBeansOnly) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::GoodMorning, false);
		}
#ifdef _HAS_UNUSED_ACHIEVEMENTS
		if (mBoard->mSpawnedDiggerZombie && !mBoard->mDiggerHasReachLeftSide) 
		{
			ReportAchievement::GiveAchievement(this, AchievementId::FaceToFace, false);
		}
		if (!mBoard->mHadPlantedNuts && mPlayerInfo->mLevel > 3)
		{
			ReportAchievement::GiveAchievement(this, AchievementId::MayNotContainNuts, true);
		}
#endif
	}
	else if (IsSurvivalMode())
	{
		if (mBoard->IsFinalSurvivalStage())
		{
			aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
			mBoard->SurvivalSaveScore();

			if (aUnlockedNewChallenge && HasFinishedAdventure())
			{
				int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_SURVIVAL);
				if (aNumTrophies != 8 && aNumTrophies != 9)
				{
					mPlayerInfo->mHasNewSurvival = true;
				}
			}
		}
	}
	else if (IsPuzzleMode())
	{
		aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
		mPlayerInfo->mChallengeRecords[GetCurrentChallengeIndex()]++;

		if (!HasFinishedAdventure() && (mGameMode == GameMode::GAMEMODE_SCARY_POTTER_3 || mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_3))
		{
			aUnlockedNewChallenge = false;
		}

		if (aUnlockedNewChallenge)
		{
			if (IsScaryPotterLevel())
			{
				mPlayerInfo->mHasNewScaryPotter = 1;
			}
			else
			{
				mPlayerInfo->mHasNewIZombie = 1;
			}
		}
	}
	else
	{
		aUnlockedNewChallenge = !HasBeatenChallenge(mGameMode);
		mPlayerInfo->mChallengeRecords[GetCurrentChallengeIndex()]++;

		if (aUnlockedNewChallenge && HasFinishedAdventure())
		{
			int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
			if (aNumTrophies <= 17)
			{
				mPlayerInfo->mHasNewMiniGame = 1;
			}
		}

		// @Patoke: implemented
		int aNumTrophies = GetNumTrophies(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
		if (aNumTrophies == 20)
			ReportAchievement::GiveAchievement(this, AchievementId::BeyondTheGrave, false);
	}

	WriteCurrentUserConfig();

	return aUnlockedNewChallenge;
}


//0x4524F0
//0x4524F0
// GOTY @Patoke: 0x4558E0
void LawnApp::CheckForGameEnd()
{
	if (mBoard == nullptr || !mBoard->mLevelComplete)
		return;

	bool aUnlockedNewChallenge = UpdatePlayerProfileForFinishingLevel();

	if (IsAdventureMode())
	{
		int aLevel = mBoard->mLevel;
		bool isReplaying = mBoard->mIsReplay;
		KillBoard();

		if (!isReplaying)
		{
			if (IsFirstTimeAdventureMode() && aLevel < 50)
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else if (aLevel == FINAL_LEVEL && mPlayerInfo->mFinishedAdventure == 1)
			{
				if (mPlayerInfo->mFinishedAdventure > 1)
				{
					ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
				}
				else
				{
					ShowAwardScreen(AwardType::AWARD_CREDITS_ZOMBIENOTE, true);
				}
			}
			else if (aLevel == 9 || aLevel == 19 || aLevel == 29 || aLevel == 39 || aLevel == 49)
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else
			{
				PreNewGame(mGameMode, false);
			}
		}
		else
		{
			DoBackToMain();
		}
	}
	else if (IsLastStandEndless(mGameMode))
	{
		mBoard->mNukeCounter = 0;
		mBoard->mChallenge->mSurvivalStage++;
		KillGameSelector();
		mBoard->InitSurvivalStage();
	}
	else if (IsSurvivalMode())
	{
		if (mBoard->IsFinalSurvivalStage())
		{
			KillBoard();

			if (aUnlockedNewChallenge && HasFinishedAdventure())
			{
				ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
			}
			else
			{
				ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_SURVIVAL);
			}
		}
		else
		{
			mBoard->mNukeCounter = 0;
			mBoard->mChallenge->mSurvivalStage++;
			KillGameSelector();
			mBoard->InitSurvivalStage();
		}
	}
	else if (IsPuzzleMode())
	{
		KillBoard();

		if (aUnlockedNewChallenge)
		{
			ShowAwardScreen(AwardType::AWARD_FORLEVEL, true);
		}
		else
		{
			ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_PUZZLE);
		}
	}
	else
	{
		KillBoard();
		ShowChallengeScreen(ChallengePage::CHALLENGE_PAGE_CHALLENGE);
	}
}


void LawnApp::UpdatePlayTimeStats()
{
	static int aLastTime = -1;

	int aTickCount = GetTickCount();
	int aSession = (aTickCount - aLastTime) / 1000;

	if (mPlayerInfo && !mPlayerInfo->mHasUsedCheatKeys && !mDebugKeysEnabled && mTodCheatKeys)
	{
		mPlayerInfo->mHasUsedCheatKeys = 1;
	}

	if (aLastTime == -1)
	{
		aLastTime = aTickCount;
		return;
	}

	if (aSession > 0)
	{
		aLastTime = aTickCount;

		if ((mBoard == nullptr || !mBoard->mPaused) && mHasFocus && mLastTimerTime - mLastUserInputTick <= 10000)
		{
			mPlayTimeActiveSession += aSession;

			if (mBoard)
			{
				mBoard->mPlayTimeActiveLevel += aSession;
			}

			if (mPlayerInfo)
			{
				mPlayerInfo->mPlayTimeActivePlayer += aSession;
			}
		}
		else
		{
			mPlayTimeInactiveSession += aSession;

			if (mBoard)
			{
				mBoard->mPlayTimeInactiveLevel += aSession;
			}

			if (mPlayerInfo)
			{
				mPlayerInfo->mPlayTimeInactivePlayer += aSession;
			}
		}
	}
}

//0x452650
void LawnApp::UpdateFrames()
{
	if ((!mActive || mMinimized) && mBoard)
	{
		mBoard->ResetFPSStats();
	}

#ifdef _DEBUG
	UpdatePlayTimeStats();
#endif

	int aUpdateCount = 1;
	if (gSlowMo)
	{
		++gSlowMoCounter;
		if (gSlowMoCounter < 4)
		{
			aUpdateCount = 0;
		}
		else
		{
			gSlowMoCounter = 0;
		}
	}
	else if (gFastMo)
	{
		aUpdateCount = 20;
	}

	for (int i = 0; i < aUpdateCount; i++)
	{
		mAppCounter++;
		
		if (mBoard)
		{
			mBoard->ProcessDeleteQueue();

			if (mBoard->mTimeStopCounter > 0)	
			{
				mBoard->mTimeStopCounter--;

				for (int j = 0; j <= 5; j++)
				{
					for (int k = 0; k < mBoard->mSeedBank->mNumPackets; k++)
					{
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
						if (mBoard->mSeedBank->mSeedPackets[k].mPacketType == SeedType::SEED_TIMESTOPPER)
							continue;
#endif

						mBoard->mSeedBank->mSeedPackets[k].Update();
					}
				}
			}

			if (mGameMode == GameMode::GAMEMODE_CHALLENGE_SPEED && !mBoard->HasLevelAwardDropped()) {
				if (mBoard->mTimeStopCounter > 0)
				{
					mBoard->mTimeStopCounter--;

					for (int j = 0; j <= 5; j++)
					{
						for (int k = 0; k < mBoard->mSeedBank->mNumPackets; k++)
						{
#ifdef _HAS_BLOOM_AND_DOOM_CONTENTS
							if (mBoard->mSeedBank->mSeedPackets[k].mPacketType == SeedType::SEED_TIMESTOPPER)
								continue;
#endif

							mBoard->mSeedBank->mSeedPackets[k].Update();
						}
					}
				}
			}
		}

		SexyApp::UpdateFrames();
		if (mMusic && mMusic->mMusicInterface)
			mMusic->MusicUpdate();
		if (mLoadingThreadCompleted && mEffectSystem)
		{
			mEffectSystem->ProcessDeleteQueue();
		}

		CheckForGameEnd();
	}
}

void LawnApp::ToggleSlowMo()
{
	gSlowMoCounter = 0;
	gSlowMo = !gSlowMo;
	gFastMo = false;
}

void LawnApp::ToggleFastMo()
{
	gSlowMo = false;
	gFastMo = !gFastMo;
}

//0x452740
void LawnApp::LoadGroup(const char* theGroupName, int theGroupAveMsToLoad)
{
	PerfTimer aTimer;
	aTimer.Start();

	mResourceManager->StartLoadResources(theGroupName);
	while (!mShutdown && !mCloseRequest && !mLoadingFailed && TodLoadNextResource())
	{
		mCompletedLoadingThreadTasks += theGroupAveMsToLoad;
	}

	if (mShutdown || mCloseRequest)
		return;

	if (mResourceManager->HadError() || !ExtractResourcesByName(mResourceManager, theGroupName))
	{
		ShowResourceError();
		mLoadingFailed = true;
	}

	int aTotalGroupWeight = mResourceManager->GetNumResources(theGroupName) * theGroupAveMsToLoad;
	int aGroupTime = max(aTimer.GetDuration(), 0);
	TraceLoadGroup(theGroupName, aGroupTime, aTotalGroupWeight, theGroupAveMsToLoad);
}

//0x4528E0
void LawnApp::LoadingThreadProc()
{
	if (!TodLoadResources("LoaderBar"))
		return;

	TodStringListLoad(_S("Properties\\LawnStrings.txt"));

	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (GetFileAttributesExA("dependency\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("dependency\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("dependency\\properties\\LawnStrings.txt"));

	if (GetFileAttributesExA("extension\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("extension\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("extension\\properties\\LawnStrings.txt"));

#ifdef _ALLOW_RESOURCE_PACKS
	if (GetFileAttributesExA("resourcepack\\properties\\LawnStrings.txt", GetFileExInfoStandard, &fileInfo) != 0 || IsFileInPakFile("resourcepack\\properties\\LawnStrings.txt"))
		TodStringListLoad(_S("resourcepack\\properties\\LawnStrings.txt"));
#endif

#ifdef _HAS_ZOMBATAR
	if (!IsScreenSaver() && !IsParticleEditor())
		TodStringListLoad(_S("Properties\\ZombatarTOS.txt"));
#endif
	
	if (mTitleScreen)
	{
		mTitleScreen->mLoaderScreenIsLoaded = true;
	}

	const char* groups[] = { "LoadingFonts", "LoadingImages", "LoadingSounds" };
	int group_ave_ms_to_load[] = { 54, 9, 54 };
	for (int i = 0; i < 3; i++)
	{
		if (IsParticleEditor()) continue;

		mNumLoadingThreadTasks += mResourceManager->GetNumResources(groups[i]) * group_ave_ms_to_load[i];
	}
	mNumLoadingThreadTasks += 636;
	if (!IsParticleEditor())
		mNumLoadingThreadTasks += GetNumPreloadingTasks();
	mNumLoadingThreadTasks += mMusic->GetNumLoadingTasks();

	PerfTimer aTimer;
	aTimer.Start();

	TodHesitationTrace("start loading");
	TodHesitationBracket aHesitationResources("Resources");
	TodHesitationTrace("loading thread start");

	LoadGroup("LoadingFonts", 54);
	LoadGroup("LoadingImages", 9);

	if (mLoadingFailed || mShutdown || mCloseRequest)
		return;

	aHesitationResources.EndBracket();
	TodTrace("loading '%s' %d ms", "resources", (int)aTimer.GetDuration());

	mMusic->MusicInit();
	int aDuration = max(aTimer.GetDuration(), 0);
	aTimer.Start();

	if (/*!IsScreenSaver() && */!IsParticleEditor())
	{
		mPoolEffect = new PoolEffect();
		mPoolEffect->PoolEffectInitialize();
		mZenGarden = new ZenGarden();
	}

	if (!IsParticleEditor())
	{
		mReanimatorCache = new ReanimatorCache();
		mReanimatorCache->ReanimatorCacheInitialize();
	}

	TodFoleyInitialize(gLawnFoleyParamArray, LENGTH(gLawnFoleyParamArray));

	TodTrace("loading '%s' %d ms", "stuff", (int)aTimer.GetDuration());
	aTimer.Start();

	TrailLoadDefinitions(gLawnTrailArray, LENGTH(gLawnTrailArray));
	TodTrace("loading '%s' %d ms", "trail", (int)aTimer.GetDuration());
	aTimer.Start();
	TodHesitationTrace("trail");

	TodParticleLoadDefinitions(gLawnParticleArray, LENGTH(gLawnParticleArray));
	//aDuration = max(aTimer.GetDuration(), 0);
	aTimer.Start();

	if (!IsParticleEditor())
		PreloadForUser();
	
	if (mLoadingFailed || mShutdown || mCloseRequest)
		return;

	//aDuration = max(aTimer.GetDuration(), 0);
	aTimer.Start();

	//GetNumPreloadingTasks();
	if (!IsParticleEditor())
		LoadGroup("LoadingSounds", 54);
	TodHesitationTrace("finished loading");
}

//0x452C60
void LawnApp::FastLoad(GameMode theGameMode)
{
	if (!mShutdown)
	{
		mWidgetManager->RemoveWidget(mTitleScreen);
		SafeDeleteWidget(mTitleScreen);
		mTitleScreen = nullptr;

		PreNewGame(theGameMode, false);
	}
}

void LawnApp::LoadingThreadCompleted()
{
}

//0x452CB0
void LawnApp::LoadingCompleted()
{
	mWidgetManager->RemoveWidget(mTitleScreen);
	SafeDeleteWidget(mTitleScreen);
	mTitleScreen = nullptr;

	mResourceManager->DeleteImage("IMAGE_TITLESCREEN");
	if (IsScreenSaver()) 
	{
		KillBoard();
		mZenGarden->UpdatePlantNeeds();
		PreNewGame(GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN, false);
	}
	else if (IsParticleEditor())
	{
		KillBoard();
		ShowParticleEditor();
	}
	else
	{
		ShowGameSelector();
	}
}

//0x452D80
void LawnApp::URLOpenFailed(const std::string& theURL)
{
	SexyAppBase::URLOpenFailed(theURL);
	KillDialog(Dialogs::DIALOG_OPENURL_WAIT);
	CopyToClipboard(theURL);

	std::string aString = 
		"Please open the following URL in your browser\n\n" + 
		theURL + 
		"\n\nFor your convenience, this URL has already been copied to your clipboard.";

	DoDialog(Dialogs::DIALOG_OPENURL_WAIT, true, _S("Open Browser"), _S("OK"), StringToSexyStringFast(aString), Dialog::BUTTONS_FOOTER);
}

//0x452EE0
void LawnApp::URLOpenSucceeded(const std::string& theURL)
{
	SexyAppBase::URLOpenSucceeded(theURL);
	KillDialog(Dialogs::DIALOG_OPENURL_WAIT);
}

//0x452F00
bool LawnApp::OpenURL(const std::string& theURL, bool shutdownOnOpen)
{
	DoDialog(
		Dialogs::DIALOG_OPENURL_WAIT, 
		true, 
		_S("Opening Browser"), 
		StringToSexyString(theURL),
		_S(""), 
		Dialog::BUTTONS_NONE
	);

	DrawDirtyStuff();

	return SexyAppBase::OpenURL(theURL, shutdownOnOpen);
}

//0x453040
void LawnApp::ConfirmQuit()
{
	SexyString aBody = TodStringTranslate(_S("[QUIT_MESSAGE]"));
	SexyString aHeader = TodStringTranslate(_S("[QUIT_HEADER]"));
	LawnDialog* aDialog = (LawnDialog*)DoDialog(Dialogs::DIALOG_QUIT, true, aHeader, aBody, _S(""), Dialog::BUTTONS_OK_CANCEL);
	aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("[QUIT_BUTTON]"));
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
}

//0x4531D0
void LawnApp::PreDisplayHook()
{
	SexyApp::PreDisplayHook();
}

void LawnApp::ButtonPress(int theId)
{
}

//0x4531E0
void LawnApp::ButtonDepress(int theId)
{
	if (theId % 10000 >= 2000 && theId % 10000 < 3000)  // 按钮编号 theId ∈ [2000, 3000) 时，表示按下 theId - 2000 编号的对话中的“是”按钮
	{
		switch (theId - 2000)
		{
		case Dialogs::DIALOG_NEW_GAME:
			KillDialog(Dialogs::DIALOG_NEW_GAME);
			ShowGameSelector();
			return;

		case Dialogs::DIALOG_NEWOPTIONS:
			KillNewOptionsDialog();
			return;

		case Dialogs::DIALOG_MORESETTINGS:
			KillMoreSettingsDialog();
			return;

		case Dialogs::DIALOG_PREGAME_NAG:
			DoRegister();
			return;

		case Dialogs::DIALOG_LOAD_GAME:
			return;

		case Dialogs::DIALOG_CONFIRM_UPDATE_CHECK:
			KillDialog(Dialogs::DIALOG_CONFIRM_UPDATE_CHECK);
			CheckForUpdates();
			return;

		case Dialogs::DIALOG_QUIT:
			KillDialog(Dialogs::DIALOG_QUIT);
			SendMessage(mHWnd, WM_CLOSE, NULL, NULL);
			return;

		case Dialogs::DIALOG_NAG:
			KillDialog(Dialogs::DIALOG_NAG);
			DoRegister();
			return;

		case Dialogs::DIALOG_INFO:
			KillDialog(Dialogs::DIALOG_INFO);
			return;

		case Dialogs::DIALOG_PAUSED:
			KillDialog(Dialogs::DIALOG_PAUSED);
			return;

		case Dialogs::DIALOG_NO_MORE_MONEY:
			KillDialog(Dialogs::DIALOG_NO_MORE_MONEY);
			mBoard->AddSunMoney(100);
			return;

		case Dialogs::DIALOG_BONUS:
			KillDialog(Dialogs::DIALOG_BONUS);
			return;

		case Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN:
			KillDialog(Dialogs::DIALOG_CONFIRM_BACK_TO_MAIN);
			mBoardResult = BoardResult::BOARDRESULT_QUIT;
			mBoard->TryToSaveGame();
			DoBackToMain();
			return;

		case Dialogs::DIALOG_USERDIALOG:
			FinishUserDialog(true);
			return;

		case Dialogs::DIALOG_CREATEUSER:
			FinishCreateUserDialog(true);
			return;

		case Dialogs::DIALOG_CONFIRMDELETEUSER:
			FinishConfirmDeleteUserDialog(true);
			return;

		case Dialogs::DIALOG_RENAMEUSER:
			FinishRenameUserDialog(true);
			return;

		case Dialogs::DIALOG_CREATEUSERERROR:
		case Dialogs::DIALOG_RENAMEUSERERROR:
			FinishNameError(theId - 2000);
			return;

		case Dialogs::DIALOG_CHEAT:
			FinishCheatDialog(true);
			return;

		case Dialogs::DIALOG_RESTARTCONFIRM:
			FinishRestartConfirmDialog();
			return;

		case Dialogs::DIALOG_TIMESUP:
			FinishTimesUpDialog();
			return;

		case 20008:
			KillDialog(20008);
			KillDialog(Dialogs::DIALOG_CHECKING_UPDATES);
			return;

		case Dialogs::DIALOG_CONFIRM_RIP_MODE:
			KillDialog(Dialogs::DIALOG_CONFIRM_RIP_MODE);
			mRIPMode = true;
			mPlayerInfo->mDidRIPMode = true;
			return;
		
		case Dialogs::DIALOG_UNLOCK:
		{
			KillDialog(Dialogs::DIALOG_UNLOCK);
			if (mPlayerInfo)
			{
				mPlayerInfo->SetLevel(1);
				mPlayerInfo->mFinishedAdventure = 2;
				mPlayerInfo->AddCoins(50000);
				mPlayerInfo->mHasUsedCheatKeys = true;
				mPlayerInfo->mHasUnlockedMinigames = true;
				mPlayerInfo->mHasUnlockedPuzzleMode = true;
				mPlayerInfo->mHasUnlockedSurvivalMode = true;

				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FERTILIZER] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_BUG_SPRAY] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_CHOCOLATE] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_FOOD] = PURCHASE_COUNT_OFFSET + 5;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GATLINGPEA] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_CATTAIL] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_WINTERMELON] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_SPIKEROCK] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_COBCANNON] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PLANT_IMITATER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PACKET_UPGRADE] = 4;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_POOL_CLEANER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_ROOF_CLEANER] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_PHONOGRAPH] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_GARDENING_GLOVE] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_MUSHROOM_GARDEN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_WHEEL_BARROW] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_AQUARIUM_GARDEN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_TREE_OF_WISDOM] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_FIRSTAID] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_GOLD_WATERINGCAN] = 1;
				mPlayerInfo->mPurchases[StoreItem::STORE_ITEM_STINKY_THE_SNAIL] = 1;

				if (mGameSelector) mGameSelector->SyncProfile(true);

				EraseFile(GetSavedGameName(GameMode::GAMEMODE_ADVENTURE, mPlayerInfo->mId));
			}
			return;
		}

		//case Dialogs::DIALOG_MORESETTINGS:
		//{
		//	MoreSettingsDialog* aDialog = (MoreSettingsDialog*)GetDialog(Dialogs::DIALOG_MORESETTINGS);
		//	if (aDialog)
		//	{
		//		mWindowCursor = !aAdvanceDialog->mCustomCursor->IsChecked();
		//		RegistryWriteBoolean("WindowCursor", mWindowCursor);
		//		EnforceCursor();
		//		bool prevFPS = mFPSToggled;
		//		mFPSToggled = aAdvanceDialog->mFPSToggle->IsChecked();
		//		RegistryWriteBoolean("FPSToggled", mFPSToggled);
		//		mShowFPS = mFPSToggled;
		//		if (prevFPS != mShowFPS && mShowFPS)
		//		{
		//			mShowFPSMode = 0;
		//			Sexy::gFPSTimer.Start();
		//			Sexy::gFrameCount = 0;
		//			Sexy::gFPSDisplay = 0;
		//			Sexy::gForceDisplay = true;
		//		}
		//		mNoAutoPause = !aAdvanceDialog->mAutoPause->IsChecked();
		//		RegistryWriteBoolean("NoAutoPause", mNoAutoPause);
		//		mShowKeybindHint = aAdvanceDialog->mKeybindHint->IsChecked();
		//		RegistryWriteBoolean("ShowKeybindHint", mShowKeybindHint);
		//		mOptimizedGameplay = aAdvanceDialog->mOptimizedGameplay->IsChecked();
		//		RegistryWriteBoolean("OptimizeGameplay", mOptimizedGameplay);
		//		mClearerSeedCost = aAdvanceDialog->mClearerSeedCost->IsChecked();
		//		RegistryWriteBoolean("ClearerSeedCost", mClearerSeedCost);
		//		mNoTooltip = !aAdvanceDialog->mShowToolTip->IsChecked();
		//		RegistryWriteBoolean("NoTooltip", mNoTooltip);
		//		mNumericRecharge = aAdvanceDialog->mNumericRecharge->IsChecked();
		//		RegistryWriteBoolean("NumericRecharge", mNumericRecharge);
		//		bool prevWide = mWideToggled;
		//		mWideToggled = aAdvanceDialog->mWideScreen->IsChecked();
		//		RegistryWriteBoolean("WideToggled", mWideToggled);
		//		mNoWarnings = !aAdvanceDialog->mShowWarnings->IsChecked();
		//		RegistryWriteBoolean("NoWarnings", mNoWarnings);

		//		bool want3D = aAdvanceDialog->mHardwareAcceleration->IsChecked();
		//		SwitchScreenMode(mIsWindowed, want3D, false);

		//	/*	if (prevWide != mWideToggled) {
		//			SwitchScreenMode(mIsWindowed, mDDInterface->mIs3D, !mPlayerInfo->mHardmodeIsOff, !mPlayerInfo->mIsNotCoop, true);
		//			if (gLawnApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO && mBoard->mCutScene) mBoard->mCutScene->RehupScreen();
		//		}*/

		//		KillDialog(Dialogs::DIALOG_MORESETTINGS);
		//		ClearUpdateBacklog();
		//	}
		//	return;
		//}

		default:
			KillDialog(theId - 2000);
			return;
		}
	}

	if (theId % 10000 >= 3000 && theId < 4000)  // 按钮编号 theId ∈ [3000, 4000) 时，表示按下 theId - 3000 编号的对话中的“否”按钮
	{
		switch (theId - 3000)
		{
		case Dialogs::DIALOG_PREGAME_NAG:
			KillDialog(Dialogs::DIALOG_PREGAME_NAG);
			Shutdown();
			return;

		case Dialogs::DIALOG_LOAD_GAME:
			KillDialog(Dialogs::DIALOG_LOAD_GAME);
			return;

		case Dialogs::DIALOG_USERDIALOG:
			FinishUserDialog(false);
			return;

		case Dialogs::DIALOG_CREATEUSER:
			FinishCreateUserDialog(false);
			return;

		case Dialogs::DIALOG_CONFIRMDELETEUSER:
			FinishConfirmDeleteUserDialog(false);
			return;

		case Dialogs::DIALOG_RENAMEUSER:
			FinishRenameUserDialog(false);
			return;

		case Dialogs::DIALOG_CHEAT:
			FinishCheatDialog(false);
			return;

		case Dialogs::DIALOG_TIMESUP:
			FinishTimesUpDialog();
			return;

		case 10008:
			KillDialog(10008);
			KillDialog(Dialogs::DIALOG_CHECKING_UPDATES);
			return;

		default:
			KillDialog(theId - 3000);
			return;
		}
	}
}

void LawnApp::CenterDialog(Dialog* theDialog, int theWidth, int theHeight)
{
	theDialog->Resize((gLawnApp->mWidth - theWidth) / 2, (gLawnApp->mHeight - theHeight) / 2, theWidth, theHeight);
}

//0x453630
void LawnApp::PlayFoley(FoleyType theFoleyType)
{
	if (!mMuteSoundsForCutscene)
	{
		mSoundSystem->PlayFoley(theFoleyType);
	}
}

//0x453650
void LawnApp::PlayFoleyPitch(FoleyType theFoleyType, float thePitch)
{
	if (!mMuteSoundsForCutscene)
	{
		mSoundSystem->PlayFoleyPitch(theFoleyType, thePitch);
	}
}

//0x453670
SexyString LawnApp::GetStageString(int theLevel)
{
	int aArea = ClampInt((abs(theLevel) - 1) / LEVELS_PER_AREA + 1, 1, ADVENTURE_AREAS + 1);
	int aSub = abs(theLevel) - (aArea - 1) * LEVELS_PER_AREA;
	SexyString aFormat;
	if (theLevel < 0) aFormat = StrFormat(_S(" -%d-%d"), aArea, aSub);
	else aFormat = StrFormat(_S(" %d-%d"), aArea, aSub);
	return aFormat;
} 

bool LawnApp::IsAdventureMode()
{
	return mGameMode == GameMode::GAMEMODE_ADVENTURE;
}

//0x4536D0
bool LawnApp::IsSurvivalMode()
{
	return mGameMode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1 && mGameMode <= GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_5 || mGameMode >= GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_6 && mGameMode <= GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_HIGHGROUND;
}

//0x4536F0
bool LawnApp::IsPuzzleMode()
{
	return
		(mGameMode >= GameMode::GAMEMODE_SCARY_POTTER_1 && mGameMode <= GameMode::GAMEMODE_SCARY_POTTER_ENDLESS) ||
		(mGameMode >= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 && mGameMode <= GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS);
}

//0x453710
bool LawnApp::IsChallengeMode()
{
	return !IsAdventureMode() && !IsPuzzleMode() && !IsSurvivalMode();
}

bool LawnApp::IsSurvivalNormal(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsSurvivalHard(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_HARD_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsSurvivalEndless(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_1;
	int aLimboLevel = theGameMode - GameMode::GAMEMODE_SURVIVAL_ENDLESS_STAGE_6;
	return aLevel >= 0 && aLevel <= 4 || aLimboLevel >= 0 && aLimboLevel <= 1;
}

bool LawnApp::IsEndlessScaryPotter(GameMode theGameMode)
{
	return theGameMode == GameMode::GAMEMODE_SCARY_POTTER_ENDLESS;
}

bool LawnApp::IsEndlessIZombie(GameMode theGameMode)
{
	return theGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS;
}

//0x453740
bool LawnApp::IsContinuousChallenge()
{
	return 
		IsArtChallenge() || 
		IsSlotMachineLevel() || 
		IsFinalBossLevel() || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED || 
		mGameMode == GameMode::GAMEMODE_UPSELL || 
		mGameMode == GameMode::GAMEMODE_INTRO || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_BEGHOULED_TWIST
#ifdef _DS_MINIGAMES
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP 
#endif
		;
}

bool LawnApp::IsArtChallenge()
{
	if (mBoard == nullptr)
		return false;

	return 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_WALLNUT || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ART_CHALLENGE_SUNFLOWER || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_SEEING_STARS;
}

//0x4537B0
bool LawnApp::IsSquirrelLevel()
{
	return mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SQUIRREL;
}

//0x4537D0
bool LawnApp::IsIZombieLevel()
{
	if (mBoard == nullptr)
		return false;

	return
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_1 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_2 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_3 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_4 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_5 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_6 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_7 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_8 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_9 ||
		mGameMode == GameMode::GAMEMODE_PUZZLE_I_ZOMBIE_ENDLESS;
}

//0x453820
bool LawnApp::IsShovelLevel()
{
	return mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SHOVEL;
}

//0x453840
bool LawnApp::IsWallnutBowlingLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING || mGameMode == GameMode::GAMEMODE_CHALLENGE_WALLNUT_BOWLING_2)
		return true;

	return IsAdventureMode() && mPlayerInfo->mLevel == 5;
}

//0x453870
bool LawnApp::IsSlotMachineLevel()
{
	return (mBoard && mGameMode == GameMode::GAMEMODE_CHALLENGE_SLOT_MACHINE);
}

//0x453890
bool LawnApp::IsWhackAZombieLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_WHACK_A_ZOMBIE)
		return true;

	return IsAdventureMode() && mPlayerInfo->mLevel == 15;
}

//0x4538C0
bool LawnApp::IsLittleTroubleLevel()
{
	if (mBoard == nullptr)
		return false;

	return mGameMode == GameMode::GAMEMODE_CHALLENGE_LITTLE_TROUBLE || (mGameMode == GameMode::GAMEMODE_ADVENTURE && mPlayerInfo->mLevel == 25);
}

//0x4538F0
bool LawnApp::IsScaryPotterLevel()
{
	if (mGameMode >= GameMode::GAMEMODE_SCARY_POTTER_1 && mGameMode <= GameMode::GAMEMODE_SCARY_POTTER_ENDLESS)
		return true;

#ifdef _MOBILE_MINIGAMES
	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_VASEBREAKER)
		return true;
#endif

	if (mBoard == nullptr)
		return false;

	return IsAdventureMode() && mPlayerInfo->mLevel == 35;
}

//0x453920
bool LawnApp::IsStormyNightLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_STORMY_NIGHT)
		return true;

	return IsAdventureMode() && mPlayerInfo->mLevel == 40;
}

//0x453950
bool LawnApp::IsBungeeBlitzLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_BUNGEE_BLITZ)
		return true;

	return IsAdventureMode() && mPlayerInfo->mLevel == 45;
}

//0x453980
bool LawnApp::IsMiniBossLevel()
{
	if (mBoard == nullptr)
		return false;

	return
		(IsAdventureMode() && mPlayerInfo->mLevel == 10) ||
		(IsAdventureMode() && mPlayerInfo->mLevel == 20) ||
		(IsAdventureMode() && mPlayerInfo->mLevel == 30) ||
		(IsAdventureMode() && mPlayerInfo->mLevel == 40);
}

//0x4539D0
bool LawnApp::IsFinalBossLevel()
{
	if (mBoard == nullptr)
		return false;

	if (mGameMode == GameMode::GAMEMODE_CHALLENGE_FINAL_BOSS)
		return true;

	return IsAdventureMode() && mPlayerInfo->mLevel == 50;
}

//0x453A00
bool LawnApp::IsChallengeWithoutSeedBank()
{
	return 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_RAINING_SEEDS || 
		mGameMode == GameMode::GAMEMODE_UPSELL || 
		mGameMode == GameMode::GAMEMODE_INTRO || 
		IsWhackAZombieLevel() || 
		IsSquirrelLevel() || 
		IsScaryPotterLevel() || 
		mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || 
		mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM 
#ifdef _MOBILE_MINIGAMES
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_BUTTERED_POPCORN
#endif
#ifdef _DS_MINIGAMES
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE
		|| mGameMode == GameMode::GAMEMODE_CHALLENGE_ZOMBIE_TRAP 
#endif
		;
}

bool LawnApp::IsNight()
{
	if (IsIceDemo() || mPlayerInfo == nullptr /*&& mBoard == nullptr*/)
		return false;

	if (mBoard)
		return (mBoard->mLevel >= 11 && mBoard->mLevel <= 20) || (mBoard->mLevel >= 31 && mBoard->mLevel <= 40) || mBoard->mLevel == 50;

	return (mPlayerInfo->mLevel >= 11 && mPlayerInfo->mLevel <= 20) || (mPlayerInfo->mLevel >= 31 && mPlayerInfo->mLevel <= 40) || mPlayerInfo->mLevel == 50;
}

int LawnApp::GetCurrentChallengeIndex()
{
	return (int)mGameMode - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
}

ChallengeDefinition& LawnApp::GetCurrentChallengeDef()
{
	return GetChallengeDefinition(GetCurrentChallengeIndex());
}

PottedPlant* LawnApp::GetPottedPlantByIndex(int thePottedPlantIndex)
{
	TOD_ASSERT(thePottedPlantIndex >= 0 && thePottedPlantIndex < mPlayerInfo->mNumPottedPlants);
	return &mPlayerInfo->mPottedPlant[thePottedPlantIndex];
}

//0x453A50
bool LawnApp::UpdateApp()
{
	if (mCloseRequest)
	{
		Shutdown();
		return false;
	}

	//if (mLoadingThreadCompleted)
	//{
	//	LoadingThreadCompleted();
	//}

	bool updated = SexyAppBase::UpdateApp();

	//if (mLoadingThreadCompleted && !mExitToTop)
	//{
	//	CheckForUpdates();
	//}

	return updated;
}

//0x453A70
void LawnApp::CloseRequestAsync()
{
	mDeferredMessages.clear();
	mExitToTop = true;
	mCloseRequest = true;
}

//0x453A90
SeedType LawnApp::GetAwardSeedForLevel(int theLevel)
{
	int aArea = (theLevel - 1) / LEVELS_PER_AREA + 1;
	int aSub = (theLevel - 1) % LEVELS_PER_AREA + 1;
	int aSeedsHasGot = (aArea - 1) * 8 + aSub;  // 一般来说，每大关可以获得 8 种植物，每小关可以获得 1 种植物
	if (aSub >= 10)
	{
		aSeedsHasGot -= 2;  // 到达第 10 小关时，本大关中有 2 小关的奖励不是新植物
	}
	else if (aSub >= 5)
	{
		aSeedsHasGot -= 1;  // 到达第 5 小关时，本大关中有 1 小关的奖励不是新植物
	}
	if (aSeedsHasGot > 40)
	{
		aSeedsHasGot = 40;
	}
	
	return (SeedType)aSeedsHasGot;
}

//0x453AC0
int LawnApp::GetSeedsAvailable()
{
	int aLevel = mBoard && mBoard->mIsReplay && mPlayerLevelRef > 4 ? mPlayerLevelRef : mPlayerInfo->GetLevel();
	int maxPlants = 49;

	if (HasFinishedAdventure() || aLevel > 50 && mPlayerInfo && mPlayerInfo->mHasUsedCheatKeys)
	{
		if (mTodCheatKeys || mDebugKeysEnabled || mPlayerInfo && mPlayerInfo->mHasUsedCheatKeys) maxPlants += NUM_SEEDS_IN_CHOOSER - SEED_IMITATER - 1;
		return maxPlants;
	}

	SeedType aSeedTypeMax = GetAwardSeedForLevel(aLevel);
	return min(maxPlants, aSeedTypeMax); 
}

//0x453B20
bool LawnApp::HasSeedType(SeedType theSeedType)
{
	if (IsTrialStageLocked() && theSeedType >= SeedType::SEED_JALAPENO)
		return false;

	/*  优化
	if (theSeedType >= SeedType::SEED_TWINSUNFLOWER && theSeedType <= SeedType::SEED_IMITATER)
		return mPlayerInfo->mPurchases[theSeedType - SeedType::SEED_GATLINGPEA];
	*/

	if (theSeedType == SeedType::SEED_GATLINGPEA)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GATLINGPEA] > 0;
	}

	if (theSeedType == SeedType::SEED_TWINSUNFLOWER)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_TWINSUNFLOWER] > 0;
	}
	if (theSeedType == SeedType::SEED_GLOOMSHROOM)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GLOOMSHROOM] > 0;
	}
	if (theSeedType == SeedType::SEED_CATTAIL)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_CATTAIL] > 0;
	}
	if (theSeedType == SeedType::SEED_WINTERMELON)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_WINTERMELON] > 0;
	}
	if (theSeedType == SeedType::SEED_GOLD_MAGNET)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_GOLD_MAGNET] > 0;
	}
	if (theSeedType == SeedType::SEED_SPIKEROCK)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_SPIKEROCK] > 0;
	}
	if (theSeedType == SeedType::SEED_COBCANNON)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_COBCANNON] > 0;
	}
	if (theSeedType == SeedType::SEED_IMITATER)
	{
		return mPlayerInfo->mPurchases[(int)StoreItem::STORE_ITEM_PLANT_IMITATER] > 0;
	}

	return theSeedType < GetSeedsAvailable();
}

bool LawnApp::SeedTypeAvailable(SeedType theSeedType)
{
	return HasSeedType(theSeedType);
}

//0x453C30
Reanimation* LawnApp::AddReanimation(float theX, float theY, int theRenderOrder, ReanimationType theReanimationType)
{
	return mEffectSystem->mReanimationHolder->AllocReanimation(theX, theY, theRenderOrder, theReanimationType);
}

//0x453C80
TodParticleSystem* LawnApp::AddTodParticle(float theX, float theY, int theRenderOrder, ParticleEffect theEffect)
{
	return mEffectSystem->mParticleHolder->AllocParticleSystem(theX, theY, theRenderOrder, theEffect);
}

ParticleSystemID LawnApp::ParticleGetID(TodParticleSystem* theParticle)
{
	return (ParticleSystemID)mEffectSystem->mParticleHolder->mParticleSystems.DataArrayGetID(theParticle);
}

ReanimationID LawnApp::ReanimationGetID(Reanimation* theReanimation)
{
	return (ReanimationID)mEffectSystem->mReanimationHolder->mReanimations.DataArrayGetID(theReanimation);
}

TodParticleSystem* LawnApp::ParticleGet(ParticleSystemID theParticleID)
{
	return mEffectSystem->mParticleHolder->mParticleSystems.DataArrayGet((unsigned int)theParticleID);
}

TodParticleSystem* LawnApp::ParticleTryToGet(ParticleSystemID theParticleID)
{
	return mEffectSystem->mParticleHolder->mParticleSystems.DataArrayTryToGet((unsigned int)theParticleID);
}

Reanimation* LawnApp::ReanimationGet(ReanimationID theReanimationID)
{
	return mEffectSystem->mReanimationHolder->mReanimations.DataArrayGet((unsigned int)theReanimationID);
}

//0x453CB0
Reanimation* LawnApp::ReanimationTryToGet(ReanimationID theReanimationID)
{
	return mEffectSystem->mReanimationHolder->mReanimations.DataArrayTryToGet((unsigned int)theReanimationID);
}

//0x453CF0
void LawnApp::RemoveReanimation(ReanimationID theReanimationID)
{
	Reanimation* aReanim = ReanimationTryToGet(theReanimationID);
	if (aReanim)
	{
		aReanim->ReanimationDie();
	}
}

void LawnApp::RemoveParticle(ParticleSystemID theParticleID)
{
	TodParticleSystem* aParticle = ParticleTryToGet(theParticleID);
	if (aParticle)
	{
		aParticle->ParticleSystemDie();
	}
}

//0x453D20
bool LawnApp::AdvanceCrazyDaveText()
{
	std::string aMessageName = StrFormat("[CRAZY_DAVE_%d]", mCrazyDaveMessageIndex + 1).c_str();
	if (!TodStringListExists(aMessageName))
	{
		return false;
	}

	CrazyDaveTalkIndex(mCrazyDaveMessageIndex + 1);
	return true;
}

//0x453DC0
SexyString LawnApp::GetCrazyDaveText(int theMessageIndex)
{
	SexyString aMessage = StrFormat(_S("[CRAZY_DAVE_%d]"), theMessageIndex);
	aMessage = TodReplaceString(aMessage, _S("{PLAYER_NAME}"), mPlayerInfo->mName);
	aMessage = TodReplaceString(aMessage, _S("{MONEY}"), GetMoneyString(mPlayerInfo->mCoins));
	int aCost = StoreScreen::GetItemCost(StoreItem::STORE_ITEM_PACKET_UPGRADE);
	aMessage = TodReplaceString(aMessage, _S("{UPGRADE_COST}"), GetMoneyString(aCost));
	return aMessage;
}

//0x454070
bool LawnApp::CanShowAlmanac()
{
	if (IsIceDemo())
		return false;

	if (mPlayerInfo == nullptr)
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mLevel >= 15;
}

//0x454090
bool LawnApp::CanShowStore()
{
	if (IsIceDemo())
		return false;

	if (mPlayerInfo == nullptr)
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mHasSeenUpsell || mPlayerInfo->mLevel >= 25;
}

//0x4540C0
bool LawnApp::CanShowZenGarden()
{
	if (mPlayerInfo == nullptr)
		return false;

	if (IsTrialStageLocked())
		return false;

	return HasFinishedAdventure() || mPlayerInfo->mLevel >= 45;
}

bool LawnApp::CanSpawnYetis()
{
	const ZombieDefinition& aZombieDef = GetZombieDefinition(ZombieType::ZOMBIE_YETI);
	return HasFinishedAdventure() && (mPlayerInfo->mFinishedAdventure >= 2 || mPlayerInfo->mLevel >= aZombieDef.mStartingLevel);
}

//0x454120
bool LawnApp::HasBeatenChallenge(GameMode theGameMode)
{
	if (mPlayerInfo == nullptr)
		return false;

	int aChallengeIndex = theGameMode - GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1;
	TOD_ASSERT(aChallengeIndex >= 0 && aChallengeIndex < NUM_CHALLENGE_MODES);
	if (IsSurvivalNormal(theGameMode))
	{
		return mPlayerInfo->mChallengeRecords[aChallengeIndex] >= SURVIVAL_NORMAL_FLAGS;
	}
	if (IsSurvivalHard(theGameMode))
	{
		return mPlayerInfo->mChallengeRecords[aChallengeIndex] >= SURVIVAL_HARD_FLAGS;
	}
	if (IsSurvivalEndless(theGameMode) || IsEndlessScaryPotter(theGameMode) || IsEndlessIZombie(theGameMode) || IsLastStandEndless(theGameMode))
	{
		return false;
	}

	if (mGameMode == GameMode::GAMEMODE_UPSELL && theGameMode > GameMode::GAMEMODE_CHALLENGE_SLOT_MACHINE) {
		return false;
	}
	return mPlayerInfo->mChallengeRecords[aChallengeIndex] > 0;
}

//0x454170
bool LawnApp::HasFinishedAdventure()
{
	return mPlayerInfo && mPlayerInfo->mFinishedAdventure > 0;
}

//0x454190
bool LawnApp::IsFirstTimeAdventureMode()
{
	return IsAdventureMode() && !HasFinishedAdventure();
}

//0x4541B0
void LawnApp::CrazyDaveEnter()
{
	TOD_ASSERT(mCrazyDaveState == CRAZY_DAVE_OFF);
	TOD_ASSERT(!ReanimationTryToGet(mCrazyDaveReanimID));

	Reanimation* aCrazyDaveReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_CRAZY_DAVE);
	aCrazyDaveReanim->mIsAttachment = true;
	aCrazyDaveReanim->SetBasePoseFromAnim("anim_idle_handing");
	mCrazyDaveReanimID = ReanimationGetID(aCrazyDaveReanim);
	aCrazyDaveReanim->PlayReanim("anim_enter", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);

	mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_ENTERING;
	mCrazyDaveMessageIndex = -1;
	mCrazyDaveMessageText.clear();
	mCrazyDaveBlinkCounter = RandRangeInt(400, 800);

	if (mGameScene == GameScenes::SCENE_LEVEL_INTRO && IsStormyNightLevel())
	{
		aCrazyDaveReanim->mColorOverride = Color(64, 64, 64);
	}
}

//0x4542F0
void LawnApp::CrazyDaveDie()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim)
	{
		aCrazyDaveReanim->ReanimationDie();

		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_OFF;
		mCrazyDaveReanimID = ReanimationID::REANIMATIONID_NULL;
		mCrazyDaveMessageIndex = -1;
		mCrazyDaveMessageText.clear();

		CrazyDaveStopSound();
	}
}

//0x454350
void LawnApp::CrazyDaveLeave()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim)
	{
		if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
		{
			CrazyDaveDoneHanding();
		}

		aCrazyDaveReanim->PlayReanim("anim_leave", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 20, 24.0f);
		aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);

		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_LEAVING;
		mCrazyDaveMessageIndex = -1;
		mCrazyDaveMessageText.clear();

		CrazyDaveStopSound();
	}
}

//0x454430
void LawnApp::CrazyDaveTalkIndex(int theMessageIndex)
{
	mCrazyDaveMessageIndex = theMessageIndex;
	SexyString aMessageText = GetCrazyDaveText(theMessageIndex);
	CrazyDaveTalkMessage(aMessageText);
}

//0x4544A0
void LawnApp::CrazyDaveDoneHanding()
{
	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);
	ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
	AttachmentDie(aHandTrackInstance->mAttachmentID);

	TodTrace("DoneHanding");
}

//0x454520
void LawnApp::CrazyDaveStopSound()
{
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_EXTRA_LONG);
	mSoundSystem->StopFoley(FoleyType::FOLEY_CRAZY_DAVE_CRAZY);
}

//0x454570
void LawnApp::CrazyDaveTalkMessage(const SexyString& theMessage)
{
	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);

	bool doHanding = false;
	if (theMessage.find(_S("{HANDING}")) != SexyString::npos)
	{
		doHanding = true;
	}
	if ((mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING) && !doHanding)
	{
		CrazyDaveDoneHanding();
	}

	bool doSound = true;
	if (theMessage.find(_S("{NO_SOUND}")) != SexyString::npos)
	{
		doSound = false;
	}
	else
	{
		CrazyDaveStopSound();
	}

	int aWordsCount = 0;
	bool isControlWord = false;
	for (int i = 0; i < theMessage.size(); i++)
	{
		if (theMessage[i] == _S('{'))
		{
			isControlWord = true;
		}
		else if (theMessage[i] == _S('}'))
		{
			isControlWord = false;
		}
		else if (!isControlWord)
		{
			aWordsCount++;
		}
	}

	aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);

	if (mCrazyDaveState != CrazyDaveState::CRAZY_DAVE_TALKING || doSound)
	{
		if (doHanding)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			if (doSound)
			{
				if (theMessage.find(_S("{SHORT_SOUND}")) != SexyString::npos)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
				}
				else if (theMessage.find(_S("{SCREAM}")) != SexyString::npos)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM);
				}
				else
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
				}
			}
			
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHAKE}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_crazy", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_CRAZY);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SCREAM}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_smalltalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SCREAM2}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_mediumtalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM_2);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_WALLNUT}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aWallnutReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_WALLNUT);
			aWallnutReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 0, 12.0f);
			TodTrace("Handed");

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachEffect* aAttachEffect = AttachReanim(aHandTrackInstance->mAttachmentID, aWallnutReanim, 100.0f, 393.0f);
			aAttachEffect->mOffset.m00 = 1.2f;
			aAttachEffect->mOffset.m11 = 1.2f;

			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SCREAM_2);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_HAMMER}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aHammerReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_HAMMER);
			aHammerReanim->PlayReanim("anim_whack_zombie", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aHammerReanim->mAnimTime = 1.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachEffect* aAttachEffect = AttachReanim(aHandTrackInstance->mAttachmentID, aHammerReanim, 62.0f, 445.0f);
			aAttachEffect->mOffset.m00 = 1.5f;
			aAttachEffect->mOffset.m11 = 1.5f;

			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_FERTILIZER}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aFertilizerReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_ZENGARDEN_FERTILIZER);
			aFertilizerReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aFertilizerReanim->mAnimRate = 0.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aFertilizerReanim, 102.0f, 412.0f);
			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_TREE_FOOD}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aTreeFoodReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_TREEOFWISDOM_TREEFOOD);
			aTreeFoodReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aTreeFoodReanim->mAnimRate = 0.0f;

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aTreeFoodReanim, 102.0f, 412.0f);
			aCrazyDaveReanim->Update();

			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else if (theMessage.find(_S("{SHOW_MONEYBAG}")) != SexyString::npos)
		{
			aCrazyDaveReanim->PlayReanim("anim_talk_handing", ReanimLoopType::REANIM_LOOP, 50, 12.0f);

			Reanimation* aMoneyBagReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_ZENGARDEN_FERTILIZER);
			aMoneyBagReanim->PlayReanim("bag", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 0, 24.0f);
			aMoneyBagReanim->mAnimRate = 0.0f;
			aMoneyBagReanim->SetImageOverride("bag", IMAGE_MONEYBAG);

			ReanimatorTrackInstance* aHandTrackInstance = aCrazyDaveReanim->GetTrackInstanceByName("Dave_handinghand");
			AttachReanim(aHandTrackInstance->mAttachmentID, aMoneyBagReanim, 90.0f, 405.0f);
			aCrazyDaveReanim->Update();
			/*
			v16 = Reanimation::GetTrackInstanceByName(v3, "Dave_handinghand");
			theAnimRate = 405.0;
			v17 = 90.0;
			*/
			if (doSound)
			{
				PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
			}

			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_TALKING;
		}
		else
		{
			if (aWordsCount < 23)
			{
				aCrazyDaveReanim->PlayReanim("anim_smalltalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_SHORT);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
			else if (aWordsCount < 52)
			{
				aCrazyDaveReanim->PlayReanim("anim_mediumtalk", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_LONG);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
			else
			{
				aCrazyDaveReanim->PlayReanim("anim_blahblah", ReanimLoopType::REANIM_PLAY_ONCE_AND_HOLD, 50, 12.0f);

				if (doSound)
				{
					PlayFoley(FoleyType::FOLEY_CRAZY_DAVE_EXTRA_LONG);
				}

				mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_TALKING;
			}
		}
	}

	mCrazyDaveMessageText = theMessage;
}

//0x454ED0
void LawnApp::CrazyDaveStopTalking()
{
	bool aDoneHanding = true;
	if (mGameMode == GameMode::GAMEMODE_UPSELL)
	{
		aDoneHanding = false;
	}
	if (aDoneHanding && mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		CrazyDaveDoneHanding();
	}

	Reanimation* aCrazyDaveReanim = ReanimationGet(mCrazyDaveReanimID);
	aCrazyDaveReanim->SetImageOverride("Dave_mouths", nullptr);
	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING && !aDoneHanding)
	{
		aCrazyDaveReanim->PlayReanim("anim_idle_handing", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_IDLING;
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		aCrazyDaveReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
		mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_IDLING;
	}

	mCrazyDaveMessageIndex = -1;
	mCrazyDaveMessageText.clear();
	CrazyDaveStopSound();
}

//0x455040
void LawnApp::UpdateCrazyDave()
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim == nullptr)
		return;

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_ENTERING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING)
	{
		if (aCrazyDaveReanim->mLoopCount > 0)
		{
			aCrazyDaveReanim->PlayReanim("anim_idle", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_IDLING;
		}
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING)
	{
		if (aCrazyDaveReanim->mLoopCount > 0)
		{
			aCrazyDaveReanim->PlayReanim("anim_idle_handing", ReanimLoopType::REANIM_LOOP, 20, 12.0f);
			mCrazyDaveState = CrazyDaveState::CRAZY_DAVE_HANDING_IDLING;
		}
	}
	else if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_LEAVING && aCrazyDaveReanim->mLoopCount > 0)
	{
		CrazyDaveDie();
	}

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_IDLING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
	{
		if (mCrazyDaveMessageText.find(_S("{MOUTH_BIG_SMILE}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH1);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_SMALL_SMILE}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH5);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_BIG_OH}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH4);
		}
		else if (mCrazyDaveMessageText.find(_S("{MOUTH_SMALL_OH}")) != std::string::npos)
		{
			aCrazyDaveReanim->SetImageOverride("Dave_mouths", IMAGE_REANIM_CRAZYDAVE_MOUTH6);
		}
	}

	if (mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_IDLING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_TALKING || 
		mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_TALKING || mCrazyDaveState == CrazyDaveState::CRAZY_DAVE_HANDING_IDLING)
	{
		mCrazyDaveBlinkCounter--;
		if (mCrazyDaveBlinkCounter <= 0)
		{
			mCrazyDaveBlinkCounter = RandRangeInt(400, 800);
			Reanimation* aBlinkReanim = AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_CRAZY_DAVE);
			aBlinkReanim->SetFramesForLayer("anim_blink");
			aBlinkReanim->mLoopType = ReanimLoopType::REANIM_PLAY_ONCE_FULL_LAST_FRAME_AND_HOLD;
			aBlinkReanim->mAnimRate = 15.0f;
			aBlinkReanim->AttachToAnotherReanimation(aCrazyDaveReanim, "Dave_head");
			aBlinkReanim->mColorOverride = aCrazyDaveReanim->mColorOverride;
			aCrazyDaveReanim->AssignRenderGroupToTrack("Dave_eye", RENDER_GROUP_HIDDEN);
			mCrazyDaveBlinkReanimID = ReanimationGetID(aBlinkReanim);
		}
	}

	Reanimation* aBlinkReanim = ReanimationTryToGet(mCrazyDaveBlinkReanimID);
	if (aBlinkReanim && aBlinkReanim->mLoopCount > 0)
	{
		aCrazyDaveReanim->AssignRenderGroupToTrack("Dave_eye", RENDER_GROUP_NORMAL);
		RemoveReanimation(mCrazyDaveBlinkReanimID);
		mCrazyDaveBlinkReanimID = ReanimationID::REANIMATIONID_NULL;
	}

	aCrazyDaveReanim->Update();
}

//0x4552F0
void LawnApp::DrawCrazyDave(Graphics* g)
{
	Reanimation* aCrazyDaveReanim = ReanimationTryToGet(mCrazyDaveReanimID);
	if (aCrazyDaveReanim == nullptr)
		return;

	if (mCrazyDaveMessageText.size())
	{
		Image* aBubbleImage = IMAGE_STORE_SPEECHBUBBLE2;
		int aPosX = 285;
		int aPosY = 20;
		if (GetDialog(Dialogs::DIALOG_STORE))
		{
			aBubbleImage = IMAGE_STORE_SPEECHBUBBLE;
			aPosX -= 180;
			aPosY -= 78;
		}
		else if (mGameMode == GameMode::GAMEMODE_UPSELL)
		{
			aPosX += 130;
			aPosY += 70;
		}
		g->DrawImage(aBubbleImage, aPosX, aPosY);

		SexyString aBubbleText = mCrazyDaveMessageText;
		Rect aRect(aPosX + 25, aPosY + 6, 233, 144);
		if (aBubbleText.find(_S("{SHAKE}")) != SexyString::npos)
		{
			aBubbleText = TodReplaceString(aBubbleText, _S("{SHAKE}"), _S(""));
			aRect.mX += rand() % 2;
			aRect.mY += rand() % 2;
		}

		bool clickToContinue = true;
		if (mGameMode == GameMode::GAMEMODE_UPSELL)
		{
			clickToContinue = false;
		}
		else if (aBubbleText.find(_S("{NO_CLICK}")) != SexyString::npos)
		{
			aBubbleText = TodReplaceString(aBubbleText, _S("{NO_CLICK}"), _S(""));
			clickToContinue = false;
		}

		TodDrawStringWrapped(g, aBubbleText, aRect, FONT_BRIANNETOD16, Color::Black, DrawStringJustification::DS_ALIGN_CENTER_VERTICAL_MIDDLE);
		if (clickToContinue)
		{
			TodDrawString(g, _S("click to continue"), aPosX + 139, aPosY + 140, FONT_PICO129, Color::Black, DrawStringJustification::DS_ALIGN_CENTER);
		}
	}

	aCrazyDaveReanim->Draw(g);
}

//0x455670
int LawnApp::GetNumPreloadingTasks()
{
	int aTaskCount = IsScreenSaver() ? 0 : 13;
	if (mPlayerInfo)
	{
		for (SeedType i = SeedType::SEED_PEASHOOTER; i < SeedType::NUM_SEED_TYPES; i = (SeedType)((int)i + 1))
		{
			if (SeedTypeAvailable(i) || HasFinishedAdventure())
			{
				aTaskCount++;
			}

		}

		for (ZombieType i = ZombieType::ZOMBIE_NORMAL; i < ZombieType::NUM_ZOMBIE_TYPES;i = (ZombieType)((int)i + 1))
		{
			if (IsScreenSaver()) break;

			if (HasFinishedAdventure() || mPlayerInfo->mLevel >= GetZombieDefinition(i).mStartingLevel)
			{
				/*if (i != ZombieType::ZOMBIE_BOSS &&
					i != ZombieType::ZOMBIE_CATAPULT &&
					i != ZombieType::ZOMBIE_GARGANTUAR &&
					i != ZombieType::ZOMBIE_DIGGER &&
					i != ZombieType::ZOMBIE_ZAMBONI)*/
				{
					aTaskCount++;
				}
			}
		}
	}
	return aTaskCount * 68;
}

//0x455720
void LawnApp::PreloadForUser()
{
	int aNumTasks = mCompletedLoadingThreadTasks + GetNumPreloadingTasks();
	if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
	{
		TodTrace("preload canceled\n");
		mNumLoadingThreadTasks = aNumTasks;
		return;
	}

	if (!IsScreenSaver())
	{
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_PUFF, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_LAWN_MOWERED_ZOMBIE, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_READYSETPLANT, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68; // 204
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_FINAL_WAVE, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SUN, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_FADE_ON, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68; // 204
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE_NEWSPAPER, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68; // 393
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SELECTOR_SCREEN, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 340; // 340
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_ZOMBIE_HAND, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_SLIDE_ON, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_TEXT_SLIDE_DOWN, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_SELECTORSCREEN_SPOTLIGHT, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 68;
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES3, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES4, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_BUSHES5, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES3, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES4, true);
		ReanimatorEnsureDefinitionLoaded(ReanimationType::REANIM_NIGHT_BUSHES5, true);
		if (mCompletedLoadingThreadTasks < aNumTasks)
			mCompletedLoadingThreadTasks += 408;
	}

	if (mPlayerInfo)
	{
		for (SeedType i = SeedType::SEED_PEASHOOTER; i < SeedType::NUM_SEED_TYPES; i = (SeedType)((int)i + 1))
		{
			if (SeedTypeAvailable(i) || HasFinishedAdventure())
			{
				Plant::PreloadPlantResources(i);
				if (mCompletedLoadingThreadTasks < aNumTasks)
				{
					mCompletedLoadingThreadTasks += 68;
				}

				if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
				{
					TodTrace("preload canceled\n");
					mNumLoadingThreadTasks = aNumTasks;
					return;
				}

				if (mShutdown || mCloseRequest)
				{
					return;
				}
			}
		}

		for (ZombieType i = ZombieType::ZOMBIE_NORMAL; i < ZombieType::NUM_ZOMBIE_TYPES;i = (ZombieType)((int)i + 1))
		{
			if (IsScreenSaver()) break;

			if (mPlayerInfo->mLevel >= GetZombieDefinition(i).mStartingLevel || HasFinishedAdventure())
			{
				if (i == ZombieType::ZOMBIE_BOSS && i == ZombieType::ZOMBIE_CATAPULT && i == ZombieType::ZOMBIE_GARGANTUAR ||
					i == ZombieType::ZOMBIE_DIGGER && i == ZombieType::ZOMBIE_ZAMBONI)
				{
					continue;
				}

				Zombie::PreloadZombieResources(i);
				if (mCompletedLoadingThreadTasks < aNumTasks)
				{
					mCompletedLoadingThreadTasks += 68;
				}

				if (mTitleScreen && mTitleScreen->mQuickLoadKey != KeyCode::KEYCODE_UNKNOWN)
				{
					TodTrace("preload canceled\n");
					mNumLoadingThreadTasks = aNumTasks;
					return;
				}

				if (mShutdown || mCloseRequest)
				{
					return;
				}
			}
		}
	}

	if (mCompletedLoadingThreadTasks != aNumTasks)
	{
		TodTrace("num preload tasks wasn't calculated correctly");
		mCompletedLoadingThreadTasks = aNumTasks;
	}
}


//0x455930
void LawnApp::EnforceCursor()
{
	if (mSEHOccured || !mMouseIn)
	{
		SDL_SetCursor(NULL);
		return;
	}

    LRESULT hitTest = SendMessage(mHWnd, WM_NCHITTEST, 0, MAKELPARAM(GET_X_LPARAM(GetMessagePos()), GET_Y_LPARAM(GetMessagePos())));
	if (hitTest != HTCLIENT)
		return;

	SDL_Cursor* newCursor = nullptr;

	if (mIsPlayingVideo)
	{
		SDL_HideCursor();
	}
	else if (mOverrideCursor)
	{
		SDL_SetCursor(newCursor);
		SDL_ShowCursor();
	}
	else
	{
		switch (mCursorNum)
		{
			//case CURSOR_POINTER: newCursor = mSDLPointerCursor; break;
			case CURSOR_HAND: newCursor = mSDLHandCursor; break;
			case CURSOR_DRAGGING: newCursor = mSDLDraggingCursor; break;
			case CURSOR_TEXT: newCursor = mSDLTextCursor; break;
			case CURSOR_CIRCLE_SLASH: newCursor = mSDLNoCursor; break;
			case CURSOR_WAIT: newCursor = mSDLWaitCursor; break;
			case CURSOR_CUSTOM:
			case CURSOR_NONE: SDL_HideCursor(); break;
			default: newCursor = IsParticleEditor() ? SDL_GetDefaultCursor() : mSDLPointerCursor; break;
		}
	}

	if (newCursor || IsParticleEditor())
	{
		SDL_ShowCursor();
		SDL_SetCursor(newCursor);
	}
}

//0x455AA0
SexyString LawnApp::Pluralize(int theCount, const SexyChar* theSingular, const SexyChar* thePlural)
{
	if (theCount == 1)
	{
		return TodReplaceNumberString(theSingular, _S("{COUNT}"), theCount);
	}

	return TodReplaceNumberString(thePlural, _S("{COUNT}"), theCount);
}

//0x455BA0
int LawnApp::GetNumTrophies(ChallengePage thePage)
{
	int aNumTrophies = 0;

	for (int i = 0; i < NUM_CHALLENGE_MODES; i++)
	{
		const ChallengeDefinition& aDef = GetChallengeDefinition(i);
		if (aDef.mPage == thePage && HasBeatenChallenge(aDef.mChallengeMode))
		{
			aNumTrophies++;
		}
	}

	return aNumTrophies;
}

//0x455C20
int LawnApp::TrophiesNeedForGoldSunflower()
{
	return 48 - GetNumTrophies(CHALLENGE_PAGE_SURVIVAL) - GetNumTrophies(CHALLENGE_PAGE_CHALLENGE) - GetNumTrophies(CHALLENGE_PAGE_PUZZLE);
}

//0x455C50
bool LawnApp::EarnedGoldTrophy()
{
	return HasFinishedAdventure() && TrophiesNeedForGoldSunflower() <= 0;
}

void LawnApp::FinishZenGardenToturial()
{
	mBoardResult = BoardResult::BOARDRESULT_WON;
	KillBoard();
	PreNewGame(GameMode::GAMEMODE_ADVENTURE, false);
}

//0x455C90
bool LawnApp::IsTrialStageLocked()
{
	if (mDebugTrialLocked)
		return true;

	if (mDRM && mDRM->QueryData())
		return false;

	return mTrialType == TrialType::TRIALTYPE_STAGELOCKED;
}

//0x455CC0
void LawnApp::InitHook()
{
#ifdef _DEBUG
	mDRM = nullptr;
#else
	mDRM = new PopDRMComm();
	mDRM->DoIPC();
	if (sexystricmp(GetString("MarketingMode", _S("")).c_str(), _S("StageLocked")) == 0)
	{
		mTrialType = TrialType::TRIALTYPE_STAGELOCKED;
		mDRM->EnableLocking();
	}
	else
	{
		mTrialType = TrialType::TRIALTYPE_NONE;
	}
#endif
}

//0x455E10
SexyString LawnApp::GetMoneyString(int theAmount)
{
	int aValue = theAmount * 10;
	if (aValue > 999999)
	{
		return StrFormat(_S("$%d,%03d,%03d"), aValue / 1000000, (aValue - aValue / 1000000 * 1000000) / 1000, aValue - aValue / 1000 * 1000);
	}
	else if (aValue > 9999)
	{
		return StrFormat(_S("$%d,%03d"), aValue / 1000, aValue - aValue / 1000 * 1000);
	}
	else
	{
		return StrFormat(_S("$%d"), aValue);
	}
}

//0x455EE0
SexyString LawnGetCurrentLevelName()
{
	if (gLawnApp == nullptr)
	{
		return _S("Before App");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_LOADING)
	{
		return _S("Game Loading");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_MENU)
	{
		return _S("Game Selector");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_AWARD)
	{
		return _S("Award Screen");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_CHALLENGE)
	{
		return _S("Challenge Screen");
	}
	if (gLawnApp->mGameScene == GameScenes::SCENE_CREDIT)
	{
		return _S("Credits");
	}
	if (gLawnApp->mBoard == nullptr)
	{
		return _S("Not Playing");
	}

	if (gLawnApp->IsFirstTimeAdventureMode())
	{
		return gLawnApp->GetStageString(gLawnApp->mBoard->mLevel);
	}
	if (gLawnApp->IsAdventureMode())
	{
		return StrFormat(_S("F%d"), gLawnApp->GetStageString(gLawnApp->mBoard->mLevel).c_str());
	}

	return gLawnApp->GetCurrentChallengeDef().mChallengeName;
}

//0x456060
bool LawnApp::CanDoPinataMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 1000;
}

//0x456080
bool LawnApp::CanDoDanceMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 500;
}

//0x4560A0
bool LawnApp::CanDoDaisyMode()
{
	if (mPlayerInfo == nullptr)
		return false;

	return mPlayerInfo->mChallengeRecords[(int)GameMode::GAMEMODE_TREE_OF_WISDOM - (int)GameMode::GAMEMODE_SURVIVAL_NORMAL_STAGE_1] >= 100;
}

//0x4560C0
void LawnApp::PlaySample(int theSoundNum)
{
	if (!mMuteSoundsForCutscene)
	{
		SexyAppBase::PlaySample(theSoundNum);
	}
}

//0x4560E0
void LawnApp::SwitchScreenMode(bool wantWindowed, bool is3d, bool force)
{
	//SexyAppBase::SwitchScreenMode(wantWindowed, is3d, force);
	bool anAppliedWindowedState = wantWindowed;
	if (force || wantWindowed != mIsWindowed)
	{
		if (!wantWindowed)
			ConfigureFullscreenDisplayMode();
		if (!SDL_SetWindowFullscreen(mSDLWindow, !wantWindowed))
		{
			TodTrace("Fullscreen switch failed: %s\n", SDL_GetError());
		}
		else
		{
			if (!SDL_SyncWindow(mSDLWindow))
				TodTrace("Fullscreen switch synchronization timed out: %s\n", SDL_GetError());
		}

		const bool isActuallyFullscreen = (SDL_GetWindowFlags(mSDLWindow) & SDL_WINDOW_FULLSCREEN) != 0;
		anAppliedWindowedState = !isActuallyFullscreen;
		if (isActuallyFullscreen == wantWindowed)
			TodTrace("Fullscreen switch was not applied by the window system.\n");
	}
	mIsWindowed = anAppliedWindowedState;

	NewOptionsDialog* aNewOptionsDialog = (NewOptionsDialog*)GetDialog(Dialogs::DIALOG_NEWOPTIONS);
	if (aNewOptionsDialog)
	{
		aNewOptionsDialog->mFullscreenCheckbox->SetChecked(!mIsWindowed, false);
	}

	ClearUpdateBacklog();
	mWidgetManager->MarkAllDirty();
	mHasPendingDraw = true;
}

/* #################################################################################################### */

void LawnApp::BetaSubmit(bool theAskForComments)
{

}

void LawnApp::BetaRecordLevelStats()
{

}

void LawnApp::BetaAddFile(std::list<std::string>& theUploadFileList, std::string theFileName, std::string theShortName)
{

}

void LawnApp::TraceLoadGroup(const char* theGroupName, int theGroupTime, int theTotalGroupWeigth, int theTaskWeight)
{

}

/* #################################################################################################### */

void LawnApp::DoHighScoreDialog()
{

}

void LawnApp::DoRegister()
{

}

void LawnApp::DoRegisterError()
{

}

bool LawnApp::CanDoRegisterDialog()
{
	return false;
}

void LawnApp::DoNeedRegisterDialog()
{

}

void LawnApp::FinishModelessDialogs()
{

}

bool LawnApp::NeedRegister()
{
	return false;
}

void LawnApp::UpdateRegisterInfo()
{

}

#ifdef _HAS_ZOMBATAR
void LawnApp::ShowZombatarTOS()
{
	KillDialog(Dialogs::DIALOG_ZOMBATARTOS);

	ZombatarTOS* aDialog = new ZombatarTOS(this);
	aDialog->mApp = this;
	CenterDialog(aDialog, aDialog->mWidth, aDialog->mHeight);
	aDialog->mY -= 16.5f;
	AddDialog(Dialogs::DIALOG_ZOMBATARTOS, aDialog);
	mWidgetManager->SetFocus(aDialog);
}
#endif

void LawnApp::ShowLanagugeScreen()
{
	mLanguageScreen = new LanguageWidget(this);
	mWidgetManager->AddWidget(mLanguageScreen);
	mWidgetManager->BringToBack(mLanguageScreen);
	mWidgetManager->SetFocus(mLanguageScreen);
}

void LawnApp::KillLanguageScreen()
{
	if (mLanguageScreen)
	{
		mWidgetManager->RemoveWidget(mLanguageScreen);
		SafeDeleteWidget(mLanguageScreen);
		mLanguageScreen = nullptr;
	}
}

bool LawnApp::ChallengeUsesMicrophone(GameMode theGameMode)
{
	return
#ifdef _DS_MINIGAMES
		theGameMode == GameMode::GAMEMODE_CHALLENGE_HEAT_WAVE; // ||
#endif
		//theGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN && gLawnApp->IsScreenSaver();
}

bool LawnApp::ChallengeHasScores(GameMode theGameMode)
{
#ifndef _HAS_SCORE_SYSTEM
	return false;
#endif
	return IsEndlessIZombie(theGameMode) || IsEndlessScaryPotter(theGameMode) || IsSurvivalEndless(theGameMode) || IsLastStandEndless(theGameMode);
}

bool LawnApp::IsLastStand() {
	bool aIsLastStand = mGameMode == GameMode::GAMEMODE_CHALLENGE_LAST_STAND;
	aIsLastStand |= (mGameMode >= GameMode::GAMEMODE_LAST_STAND_STAGE_1 && mGameMode <= GameMode::GAMEMODE_LAST_STAND_STAGE_5);
	aIsLastStand |= IsLastStandEndless(mGameMode);
	return aIsLastStand;
}

bool LawnApp::IsLastStandEndless(GameMode theGameMode)
{
	int aLevel = theGameMode - GameMode::GAMEMODE_LAST_STAND_ENDLESS_STAGE_1;
	return aLevel >= 0 && aLevel <= 4;
}

void LawnApp::ShowParticleEditor()
{
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
	}

	mParticleScreen = new ParticleScreen(this);
	mParticleScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mParticleScreen);
	mWidgetManager->BringToBack(mParticleScreen);
	mWidgetManager->SetFocus(mParticleScreen);
}

bool LawnApp::TryToInitializePA()
{
	PaError err = Pa_Initialize();
	if (err != paNoError)
		return false;

	err = Pa_OpenDefaultStream(&mPortAudioStream, 1, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, AudioCallback, &mVoiceVolume);
	if (err != paNoError)
		return false;

	err = Pa_StartStream(mPortAudioStream);
	if (err != paNoError)
		return false;

	return true;
}


void LawnApp::DoConfirmRIPMode()
{
	LawnDialog* aDialog = (LawnDialog*)DoDialog(
		Dialogs::DIALOG_CONFIRM_RIP_MODE,
		true,
		_S("Turn On R.I.P. Mode?"),
		_S("Are you sure you are up to the challenge?\nThe game will be extra difficult- one game over and you're back to the beginning."),
		_S(""),
		Dialog::BUTTONS_YES_NO
	);
}

void LawnApp::DoMoreSettingsDialog()
{
	MoreSettingsDialog* anExistingDialog = (MoreSettingsDialog*)GetDialog(Dialogs::DIALOG_MORESETTINGS);
	if (anExistingDialog != nullptr)
	{
		mWidgetManager->SetFocus(anExistingDialog);
		return;
	}

	MoreSettingsDialog* aDialog = new MoreSettingsDialog(this);
	AddDialog(Dialogs::DIALOG_MORESETTINGS, aDialog);
	mWidgetManager->SetFocus(aDialog);
}

void LawnApp::KillMoreSettingsDialog()
{
	MoreSettingsDialog* aDialog = (MoreSettingsDialog*)GetDialog(Dialogs::DIALOG_MORESETTINGS);
	if (aDialog == nullptr)
		return;

	const bool aDisplayRestartIsRequired = aDialog->RequiresDisplayRestart();
	RegistryWriteInteger(_S("HDRPaperWhitePercent"), mHDRPaperWhitePercent);
	RegistryWriteInteger(_S("HDRExposureTenthsEV"), mHDRExposureTenthsEV);
	RegistryWriteBoolean(_S("HDRAdaptiveToneMapping"), mHDRAdaptiveToneMapping);
	RegistryWriteInteger(_S("PreferredRefreshRateMilliHz"), mPreferredRefreshRateMilliHz);
	RegistryWriteBoolean(_S("UseExclusiveFullscreen"), mUseExclusiveFullscreen);
	RegistryWriteBoolean(_S("UseIntegerScaling"), mUseIntegerScaling);
	RegistryWriteBoolean(_S("ShowFPS"), mShowFPS);
	KillDialog(Dialogs::DIALOG_MORESETTINGS);
	ClearUpdateBacklog();

	if (aDisplayRestartIsRequired)
	{
		DoDialog(
			Dialogs::DIALOG_INFO,
			true,
			_S("[ADVANCED_DISPLAY_RESTART_HEADER]"),
			_S("[ADVANCED_DISPLAY_RESTART_BODY]"),
			_S("[OK_LABEL]"),
			Dialog::BUTTONS_FOOTER
		);
	}
}
