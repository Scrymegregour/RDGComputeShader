#include "DesaturateComputeShader.h"

#include "Interfaces/IPluginManager.h"
#include "DesaturateCS.h"

#define LOCTEXT_NAMESPACE "FDesaturateComputeShaderModule"

void FDesaturateComputeShaderModule::StartupModule()
{
	//Virtual Shader Path
	const FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("DesaturateComputeShader"))->GetBaseDir();
	const FString ShaderDir = FPaths::Combine(BaseDir, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/CustomShaders1"), ShaderDir);

	//[YourPlugin].cpp
	IRendererModule& RenderModule = FModuleManager::LoadModuleChecked<IRendererModule>("Renderer");

	PostOpaqueHandle = RenderModule.RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateLambda([](FPostOpaqueRenderParameters& Parameters)
		{
			const FViewInfo& View = *Parameters.View;
			FRDGBuilder& GraphBuilder = *Parameters.GraphBuilder;
			FRDGTextureRef SceneColor = Parameters.ColorTexture;

			if (!SceneColor)
			{
				return;
			}

			TShaderMapRef<FDesaturateCS> DesaturateShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			FRDGTextureRef InputCopy = GraphBuilder.CreateTexture(SceneColor->Desc, TEXT("DesaturateInput"));
			FRDGTextureUAVRef SceneColorUAV = GraphBuilder.CreateUAV(SceneColor);
			const FIntPoint Resolution = SceneColor->Desc.Extent;

			FDesaturateCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FDesaturateCS::FParameters>();
			PassParameters->InputTexture = InputCopy;
			PassParameters->InputSampler = TStaticSamplerState<SF_Point>::GetRHI();
			PassParameters->OutputTexture = SceneColorUAV;

			FIntVector ThreadGroupCount = FIntVector(
				FMath::DivideAndRoundUp(Resolution.X, 8),
				FMath::DivideAndRoundUp(Resolution.Y, 8),
				1);

			//uncomment if we want desaturated shader to apply at postOpaque
			/*AddCopyTexturePass(GraphBuilder, SceneColor, InputCopy);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("RDG Desaturate"),
				DesaturateShader,
				PassParameters,
				ThreadGroupCount);*/
		}));
}	

void FDesaturateComputeShaderModule::ShutdownModule()
{
	if (IRendererModule* RendererModule =
		FModuleManager::GetModulePtr<IRendererModule>("Renderer"))
	{
		RendererModule->RemovePostOpaqueRenderDelegate(PostOpaqueHandle);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDesaturateComputeShaderModule, DesaturateComputeShader)