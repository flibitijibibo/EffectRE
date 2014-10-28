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
	public sealed class EffectAnnotation
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

		#endregion

		#region Public Methods

		public bool GetValueBoolean()
		{
		}

		public int GetValueInt32()
		{
		}

		public Matrix GetValueMatrix()
		{
		}

		public float GetValueSingle()
		{
		}

		public string GetValueString()
		{
		}

		public Vector2 GetValueVector2()
		{
		}

		public Vector3 GetValueVector3()
		{
		}

		public Vector4 GetValueVector4()
		{
		}

		#endregion
	}
}
