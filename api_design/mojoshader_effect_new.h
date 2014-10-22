/* Effect parameter interface... */

/* Set the constant value for the specified effect parameter.
 *
 * This function maps to ID3DXEffect::SetRawValue.
 *
 * (parameter) is a parameter obtained from a MOJOSHADER_effect*.
 * (data) is the constant values to be applied to the parameter.
 * (offset) is the offset, in bytes, of the parameter data being modified.
 * (len) is the size, in bytes, of the data buffer being applied.
 *
 * This function is thread safe.
 */
void MOJOSHADER_effectSetRawValueHandle(const MOJOSHADER_effectParameter *parameter,
                                        const void *data,
                                        const unsigned int offset,
                                        const unsigned int len);

/* Set the constant value for the effect parameter, specified by name.
 *  Note: this function is slower than MOJOSHADER_effectSetRawValueHandle(),
 *  but we still provide it to fully map to ID3DXEffect.
 *
 * This function maps to ID3DXEffect::SetRawValue.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 * (name) is the human-readable name of the parameter being modified.
 * (data) is the constant values to be applied to the parameter.
 * (offset) is the offset, in bytes, of the parameter data being modified.
 * (len) is the size, in bytes, of the data buffer being applied.
 *
 * This function is thread safe.
 */
void MOJOSHADER_effectSetRawValueName(const MOJOSHADER_effect *effect,
                                      const char *name,
                                      const void *data,
                                      const unsigned int offset,
                                      const unsigned int len);

/* Effect technique interface... */

/* Get the current technique in use by an effect.
 *
 * This function maps to ID3DXEffect::GetCurrentTechnique.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 *
 * This function returns the technique currently used by the given effect.
 *
 * This function is thread safe.
 */
MOJOSHADER_effectTechnique *MOJOSHADER_effectGetCurrentTechnique(const MOJOSHADER_effect *effect);

/* Set the current technique to be used an effect.
 *
 * This function maps to ID3DXEffect::SetTechnique.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 * (technique) is the technique to be used by the effect when rendered.
 *
 * This function is thread safe.
 */
void MOJOSHADER_effectSetTechnique(const MOJOSHADER_effect *effect,
                                   const MOJOSHADER_effectTechnique *technique);

/* Get the next technique in an effect's list.
 *
 * This function maps to ID3DXEffect::FindNextValidTechnique.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 * (technique) can either be a technique found in the given effect, or NULL to
 *  find the first technique in the given effect.
 *
 * This function returns either the next technique after the passed technique,
 *  or the first technique if the passed technique is NULL.
 *
 * This function is thread safe.
 */
MOJOSHADER_effectTechnique *MOJOSHADER_effectFindNextValidTechnique(const MOJOSHADER_effect *effect,
                                                                    const MOJOSHADER_technique *technique
);

/* Effect Begin/End interface... */

/* Used to select which render states should be preserved by the effect pass */
typedef enum MOJOSHADER_effectSaveState
{
    MOJOSHADER_DONOTSAVESTATE =        0x00000001,
    MOJOSHADER_DONOTSAVESAMPLERSTATE = 0x00000002,
    MOJOSHADER_DONOTSAVESHADERSTATE =  0x00000003
} MOJOSHADER_effectSaveState;

/* Used to acquire the desired render state by the effect pass, and to restore
 * state changes introduced by the effect pass.
 */
typedef struct MOJOSHADER_effectRenderState
{
    /* TODO: Taking total control of the renderer seems like a bad idea.
     * Instead, we can give the application what state the Effect wants,
     * then the application can update the state as needed.
     * -flibit
     */
} MOJOSHADER_effectRenderState;

/* Prepare the effect for rendering with the currently applied technique.
 *
 * This function maps to ID3DXEffect::Begin.
 *
 * In addition to the expected Begin parameters, we also include a parameter
 *  to pass in a MOJOSHADER_effectRenderState. Rather than change the render
 *  state within MojoShader itself we will simply provide what the effect wants
 *  and allow you to use this information with your own renderer.
 *
 * You should expect the renderState to change in the following ways:
 *  - MOJOSHADER_*EffectBeginPass will update with the render state desired by
 *    the current effect pass.
 *  - MOJOSHADER_*EffectCommitChanges will update with render state changes
 *    made within the active effect pass.
 *  - MOJOSHADER_*EffectEndPass will update with the restored render state,
 *    depending on what state was asked to be saved.
 *  - MOJOSHADER_effectEnd will update with the final render state, restoring
 *    everything that was asked to be saved when this function was called.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 * (numPasses) will be filled with the number of passes that this technique
 *  will need to fully render.
 * (saveState) is a flag informing the effect what rendering states to preserve
 *  after rendering the technique as well as each individual pass.
 * (renderState) will be filled by the effect to inform you of the render state
 *  changes introduced by the technique and its passes.
 *
 * This function is thread safe.
 */
void MOJOSHADER_effectBegin(const MOJOSHADER_effect *effect,
                            unsigned int *numPasses,
                            MOJOSHADER_effectSaveState saveState,
                            const MOJOSHADER_effectRenderState *renderState);

/* Complete rendering the effect technique, and restore the render state.
 *
 * This function maps to ID3DXEffect::End.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 *
 * This function is thread safe.
 */
void MOJOSHADER_effectEnd(const MOJOSHADER_effect *effect);

/* OpenGL effect interface... */

/* Fully compile/link the shaders found within the effect.
 *
 * The MOJOSHADER_glEffect* is solely for use within the OpenGL-specific calls.
 *  In all other cases you will be using the MOJOSHADER_effect* instead.
 *
 * In a typical use case, you will be calling this immediately after obtaining
 *  the MOJOSHADER_effect*.
 *
 * (effect) is a MOJOSHADER_effect* obtained from MOJOSHADER_parseEffect().
 *
 * This function returns a MOJOSHADER_glEffect*, containing OpenGL-specific
 *  data for an accompanying MOJOSHADER_effect*.
 *
 * This call is NOT thread safe! As most OpenGL implementations are not thread
 * safe, you should probably only call this from the same thread that created
 * the GL context.
 */
MOJOSHADER_glEffect *MOJOSHADER_glCompileEffect(const MOJOSHADER_effect *effect);

/* Delete the shaders that were allocated for an effect.
 *
 * (glEffect) is a MOJOSHADER_glEffect* obtained from
 *  MOJOSHADER_glCompileEffect().
 *
 * This call is NOT thread safe! As most OpenGL implementations are not thread
 * safe, you should probably only call this from the same thread that created
 * the GL context.
 */
void MOJOSHADER_glDeleteEffect(const MOJOSHADER_glEffect *effect);

/* Begin an effect pass from the currently applied technique.
 *
 * This function maps to ID3DXEffect::BeginPass.
 *
 * (glEffect) is a MOJOSHADER_glEffect* obtained from
 *  MOJOSHADER_glCompileEffect().
 * (pass) is the index of the effect pass as found in the current technique.
 *
 * This call is NOT thread safe! As most OpenGL implementations are not thread
 * safe, you should probably only call this from the same thread that created
 * the GL context.
 */
void MOJOSHADER_glEffectBeginPass(const MOJOSHADER_glEffect *glEffect,
                                  unsigned int pass);

/* Push render state changes that occurred within an actively rendering pass.
 *
 * This function maps to ID3DXEffect::CommitChanges.
 *
 * (glEffect) is a MOJOSHADER_glEffect* obtained from
 *  MOJOSHADER_glCompileEffect().
 *
 * This call is NOT thread safe! As most OpenGL implementations are not thread
 * safe, you should probably only call this from the same thread that created
 * the GL context.
 */
void MOJOSHADER_glEffectCommitChanges(const MOJOSHADER_glEffect *glEffect);

/* End an effect pass from the currently applied technique.
 *
 * This function maps to ID3DXEffect::EndPass.
 *
 * (glEffect) is a MOJOSHADER_glEffect* obtained from
 *  MOJOSHADER_glCompileEffect().
 *
 * This call is NOT thread safe! As most OpenGL implementations are not thread
 * safe, you should probably only call this from the same thread that created
 * the GL context.
 */
void MOJOSHADER_glEffectEndPass(const MOJOSHADER_glEffect *glEffect);