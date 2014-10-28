#region License
/* FNA - XNA4 Reimplementation for Desktop Platforms
 * Copyright 2009-2014 Ethan Lee and the MonoGame Team
 *
 * Released under the Microsoft Public License.
 * See LICENSE for details.
 */
#endregion

namespace Microsoft.Xna.Framework.Graphics
{
	public sealed class EffectParameter
	{
		#region Public Properties

		public string Name
		{
			get;
			private set;
		}

		public string Semantic
		{
			get;
			private set;
		}

		public int RowCount
		{
			get;
			private set;
		}

		public int ColumnCount
		{
			get;
			private set;
		}

		public EffectParameterClass ParameterClass
		{
			get;
			private set;
		}

		public EffectParameterType ParameterType
		{
			get;
			private set;
		}

		public EffectParameterCollection StructureMembers
		{
			get;
			private set;
		}

		public EffectAnnotationCollection Annotations
		{
			get;
			private set;
		}

		#endregion

		#region Public Get Methods

		public bool GetValueBoolean()
		{
		}

		public bool[] GetValueBooleanArray(int count)
		{
		}

		public int GetValueInt32()
		{
		}

		public int[] GetValueInt32Array(int count)
		{
		}

		public Matrix GetValueMatrix()
		{
		}

		public Matrix[] GetValueMatrixArray(int count)
		{
		}

		public Matrix GetValueMatrixTranspose()
		{
		}

		public Matrix[] GetValueMatrixTransposeArray(int count)
		{
		}

		public Quaternion GetValueQuaternion()
		{
		}

		public Quaternion[] GetValueQuaternionArray(int count)
		{
		}

		public float GetValueSingle()
		{
		}

		public float[] GetValueSingleArray(int count)
		{
		}

		public string GetValueString()
		{
		}

		public Texture2D GetValueTexture2D()
		{
		}

		public Texture3D GetValueTexture3D()
		{
		}

		public TextureCube GetValueTextureCube()
		{
		}

		public Vector2 GetValueVector2()
		{
		}

		public Vector2[] GetValueVector2Array(int count)
		{
		}

		public Vector3 GetValueVector3()
		{
		}

		public Vector3[] GetValueVector3Array(int count)
		{
		}

		public Vector4 GetValueVector4()
		{
		}

		public Vector4[] GetValueVector4Array(int count)
		{
		}

		#endregion

		#region Public Set Methods

		public void SetValue(bool value)
		{
		}

		public void SetValue(bool value)
		{
		}

		public void SetValue(int value)
		{
		}

		public void SetValue(int[] value)
		{
		}

		public void SetValue(Matrix value)
		{
		}

		public void SetValue(Matrix[] value)
		{
		}

		public void SetValueTranspose(Matrix value)
		{
		}

		public void SetValueTranspose(Matrix[] value)
		{
		}

		public void SetValue(Quaternion value)
		{
		}

		public void SetValue(Quaternion[] value)
		{
		}

		public void SetValue(float value)
		{
		}

		public void SetValue(float[] value)
		{
		}

		public void SetValue(string value)
		{
		}

		public void SetValue(Texture value)
		{
		}

		public void SetValue(Vector2 value)
		{
		}

		public void SetValue(Vector2[] value)
		{
		}

		public void SetValue(Vector3 value)
		{
		}

		public void SetValue(Vector3[] value)
		{
		}

		public void SetValue(Vector4 value)
		{
		}

		public void SetValue(Vector4[] value)
		{
		}

		#endregion
	}
}
