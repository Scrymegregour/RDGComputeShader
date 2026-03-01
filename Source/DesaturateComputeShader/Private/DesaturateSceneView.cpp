#include "DesaturateSceneView.h"
#include "DesaturateCS.h"
#include "PostProcess/PostProcessing.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "SceneView.h"
#include "ScreenPass.h"

DesaturateSceneView::DesaturateSceneView(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{

}

void DesaturateSceneView::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	checkSlow(View.bIsViewInfo);
	const FIntRect Viewport = static_cast<const FViewInfo&>(View).ViewRect;

	Inputs.Validate();

	FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, Viewport);

	if (!SceneColor.IsValid())
	{
		return;
	}

	FRDGTextureDesc InputDesc = SceneColor.Texture->Desc;
	InputDesc.Reset();
	InputDesc.Flags |= TexCreate_UAV | TexCreate_ShaderResource;

	FRDGTextureRef InputCopy = GraphBuilder.CreateTexture(InputDesc, TEXT("DesaturateInput"));
	FRDGTextureUAVRef SceneColorUAV = GraphBuilder.CreateUAV(SceneColor.Texture);
	
	check(InputCopy);
	check(SceneColorUAV);

	const FIntPoint Resolution = InputCopy->Desc.Extent;

	TShaderMapRef<FDesaturateCS> DesaturateShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	FDesaturateCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FDesaturateCS::FParameters>();
	PassParameters->InputTexture = InputCopy;
	PassParameters->InputSampler = TStaticSamplerState<SF_Point>::GetRHI();
	PassParameters->OutputTexture = SceneColorUAV;

	FIntVector ThreadGroupCount = FIntVector(
		FMath::DivideAndRoundUp(Resolution.X, 8),
		FMath::DivideAndRoundUp(Resolution.Y, 8),
		1);

	AddCopyTexturePass(GraphBuilder, SceneColor.Texture, InputCopy);

	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("RDG Desaturate PrePostProcess"),
		DesaturateShader,
		PassParameters,
		ThreadGroupCount);
}
