const modelScene = `
{
	"metadata": {
		"version": 4.5,
		"type": "Object",
		"generator": "Object3D.toJSON"
	},
	"geometries": [
		{
			"uuid": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
			"type": "CylinderBufferGeometry",
			"radiusTop": 0.3,
			"radiusBottom": 0.3,
			"height": 4,
			"radialSegments": 32,
			"heightSegments": 1,
			"openEnded": false
		},
		{
			"uuid": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
			"type": "CylinderBufferGeometry",
			"radiusTop": 0.2,
			"radiusBottom": 0.2,
			"height": 4,
			"radialSegments": 32,
			"heightSegments": 1,
			"openEnded": false
		},
		{
			"uuid": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
			"type": "CylinderBufferGeometry",
			"radiusTop": 0.4,
			"radiusBottom": 0.4,
			"height": 1,
			"radialSegments": 8,
			"heightSegments": 1,
			"openEnded": false
		},
		{
			"uuid": "8FFD08DD-A9F1-4815-9775-2664072EEB72",
			"type": "CylinderBufferGeometry",
			"radiusTop": 0.1,
			"radiusBottom": 0.1,
			"height": 6,
			"radialSegments": 8,
			"heightSegments": 1,
			"openEnded": false
		},
		{
			"uuid": "F8E96027-3F63-4D02-B7AD-469E519D5C34",
			"type": "BoxBufferGeometry",
			"width": 7,
			"height": 0.2,
			"depth": 0.2,
			"widthSegments": 1,
			"heightSegments": 1,
			"depthSegments": 1
		},
		{
			"uuid": "E2AA4E97-0106-4697-BA9F-51BD2C168B3C",
			"type": "BoxBufferGeometry",
			"width": 5,
			"height": 0.2,
			"depth": 0.2,
			"widthSegments": 1,
			"heightSegments": 1,
			"depthSegments": 1
		},
		{
			"uuid": "0D7558F7-4B9F-4A5F-A290-8799EDB67DB1",
			"type": "BoxBufferGeometry",
			"width": 4,
			"height": 0.2,
			"depth": 0.2,
			"widthSegments": 1,
			"heightSegments": 1,
			"depthSegments": 1
		},
		{
			"uuid": "4CF57387-5CDC-4641-A9DD-B972BD5DC3EC",
			"type": "BoxBufferGeometry",
			"width": 3,
			"height": 0.2,
			"depth": 0.2,
			"widthSegments": 1,
			"heightSegments": 1,
			"depthSegments": 1
		}],
	"materials": [
		{
			"uuid": "84409DE8-994B-41D5-BFBB-609D40A37DE8",
			"type": "MeshStandardMaterial",
			"color": 16777215,
			"roughness": 0.5,
			"metalness": 0.5,
			"emissive": 15000804,
			"depthFunc": 3,
			"depthTest": true,
			"depthWrite": true
		},
		{
			"uuid": "C3D594EA-3E51-4A49-861A-FE8014FFB443",
			"type": "MeshStandardMaterial",
			"color": 16777215,
			"roughness": 0.5,
			"metalness": 0.5,
			"emissive": 16711808,
			"depthFunc": 3,
			"depthTest": true,
			"depthWrite": true
		},
		{
			"uuid": "B0814284-82A4-4E21-9C3D-BA4BE427A498",
			"type": "MeshStandardMaterial",
			"color": 16777215,
			"roughness": 0.5,
			"metalness": 0.5,
			"emissive": 16711680,
			"depthFunc": 3,
			"depthTest": true,
			"depthWrite": true
		},
		{
			"uuid": "5B3F0D23-1D52-449C-A8D5-A16BB8DAF2AB",
			"type": "MeshStandardMaterial",
			"color": 857370,
			"roughness": 0.5,
			"metalness": 0.5,
			"emissive": 65280,
			"depthFunc": 3,
			"depthTest": true,
			"depthWrite": true
		},
		{
			"uuid": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65",
			"type": "MeshStandardMaterial",
			"color": 16777215,
			"roughness": 0.5,
			"metalness": 0.5,
			"emissive": 0,
			"depthFunc": 3,
			"depthTest": true,
			"depthWrite": true
		}],
	"object": {
		"uuid": "F16858F6-2ED8-4040-99D0-84FBF78A53C7",
		"type": "Scene",
		"name": "Scene",
		"layers": 1,
		"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
		"children": [
			{
				"uuid": "38F4DFCB-6AE1-448F-9E19-A0AB91778FED",
				"type": "Group",
				"name": "11_lineActuator",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
				"children": [
					{
						"uuid": "A6623526-ECCF-4B38-B060-7326DDC974F1",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "28BDD757-BF50-464B-ABE3-A7E94D998804",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "E89293A7-AA9D-44EA-BEDC-6D32CD933E3E",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "813109F6-E0EA-4DAE-8046-5E58F714408A",
				"type": "Group",
				"name": "12_lineActuator",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,2.5,0,0,1],
				"children": [
					{
						"uuid": "607E83E2-9A87-4CA5-BF86-20C7569EE649",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "F33693F4-FBDE-4CC0-8227-96D569680220",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "B0EC03AD-F9A1-4971-AAA1-A14909B46D3D",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "48E1A7D0-BD5D-4C33-AA7F-97367A74EE38",
				"type": "Group",
				"name": "13_lineActuator",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,-2.5,0,0,1],
				"children": [
					{
						"uuid": "CD1C8268-4621-4CF3-9481-EA51FCC4BEDC",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "9222D3C4-0900-491D-99B5-2E15D39BB4E1",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "1B197079-D687-4B73-859C-A408EAB2DA0E",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "C1CAF74A-580E-41D4-A801-926AF19E65C7",
				"type": "Group",
				"name": "21_lineActuator",
				"layers": 1,
				"matrix": [-1,0,0,0,0,1,0,0,0,0,-1,0,0,0,3,1],
				"children": [
					{
						"uuid": "668A7F9E-4A4B-45FC-9B74-674F83852872",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "6380DF0C-E2E0-480F-ABCC-CC16081BA19D",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "60CB243B-A8B1-4F72-8091-89C9F6A617C0",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "6611E9BA-20A1-4EEA-B486-DD4AD5692C4A",
				"type": "Group",
				"name": "22_lineActuator",
				"layers": 1,
				"matrix": [-1,0,0,0,0,1,0,0,0,0,-1,0,2.5,0,3,1],
				"children": [
					{
						"uuid": "7440AEC7-2962-4C9D-B65A-DE61481EBAD8",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "C9B843DC-9319-4DCD-B864-334D4DA82BA7",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "16E09FE1-4815-41E2-8F99-B6EB4C94252F",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "D89BE528-CB88-4C62-8E48-AD4D6685F09C",
				"type": "Group",
				"name": "23_lineActuator",
				"layers": 1,
				"matrix": [-1,0,0,0,0,1,0,0,0,0,-1,0,-2.5,0,3,1],
				"children": [
					{
						"uuid": "2D21AF47-2D06-4FCC-AC77-E01DB5B7690B",
						"type": "Mesh",
						"name": "fix",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,6,0,1],
						"geometry": "E9D1A181-0D69-4D24-B1BC-67DD872B5F3D",
						"material": "84409DE8-994B-41D5-BFBB-609D40A37DE8"
					},
					{
						"uuid": "4EF1F9B1-8656-47F9-9FB8-D438CE8BA626",
						"type": "Mesh",
						"name": "stroke",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1],
						"geometry": "2B0B7EA4-481A-458F-B3E5-F6D109EB54DB",
						"material": "C3D594EA-3E51-4A49-861A-FE8014FFB443"
					},
					{
						"uuid": "C254FE7F-27FB-48CD-AAAF-B8075B3D7B67",
						"type": "Mesh",
						"name": "motor",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,7.5,0.35,1],
						"geometry": "9A11ACE5-C22B-4DA4-BC00-EB1F25798228",
						"material": "B0814284-82A4-4E21-9C3D-BA4BE427A498"
					}]
			},
			{
				"uuid": "2AC28075-1586-4518-AE6C-F63A1D6BBDAD",
				"type": "Group",
				"name": "body",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
				"children": [
					{
						"uuid": "9DF8FEF2-C3DF-4254-B12B-E41E36FD6BD1",
						"type": "Group",
						"name": "lineAxe",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
						"children": [
							{
								"uuid": "60F13D70-4C08-4AFD-AE8C-48FA81A4E747",
								"type": "Mesh",
								"name": "1_guide",
								"layers": 1,
								"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,0,4.5,0.5,1],
								"geometry": "8FFD08DD-A9F1-4815-9775-2664072EEB72",
								"material": "5B3F0D23-1D52-449C-A8D5-A16BB8DAF2AB"
							},
							{
								"uuid": "92F36CDF-E8FC-4ED9-9958-754B23727A80",
								"type": "Mesh",
								"name": "2_guide",
								"layers": 1,
								"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,0,4.5,2.5,1],
								"geometry": "8FFD08DD-A9F1-4815-9775-2664072EEB72",
								"material": "5B3F0D23-1D52-449C-A8D5-A16BB8DAF2AB"
							},
							{
								"uuid": "A6CC8298-D281-48F1-A672-835A6BA1101E",
								"type": "Mesh",
								"name": "3_guide",
								"layers": 1,
								"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,0,8.3,3,1],
								"geometry": "8FFD08DD-A9F1-4815-9775-2664072EEB72",
								"material": "5B3F0D23-1D52-449C-A8D5-A16BB8DAF2AB"
							},
							{
								"uuid": "5DF9CEEB-BF4A-4295-B64C-7F25D7D1755B",
								"type": "Mesh",
								"name": "4_guide",
								"layers": 1,
								"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,0,8.3,0,1],
								"geometry": "8FFD08DD-A9F1-4815-9775-2664072EEB72",
								"material": "5B3F0D23-1D52-449C-A8D5-A16BB8DAF2AB"
							}]
					},
					{
						"uuid": "0A0B92FB-1F09-4E2F-8155-3834C0B5F2FB",
						"type": "Group",
						"name": "frame_L",
						"layers": 1,
						"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
						"children": [
							{
								"uuid": "06974429-3ACD-4180-966F-6302D0FD916A",
								"type": "Mesh",
								"name": "1_L_frame",
								"layers": 1,
								"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,5.5,0.5,1],
								"geometry": "F8E96027-3F63-4D02-B7AD-469E519D5C34",
								"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
							},
							{
								"uuid": "064B205D-1BBB-4EB5-B15C-76F1396776D9",
								"type": "Mesh",
								"name": "2_L_frame",
								"layers": 1,
								"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0,5.5,2.5,1],
								"geometry": "F8E96027-3F63-4D02-B7AD-469E519D5C34",
								"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
							}]
					}]
			},
			{
				"uuid": "0FCE98B7-6F0E-4F13-9CB1-584A3A0EBA44",
				"type": "Group",
				"name": "frame_front",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,0.5,0,0,1],
				"children": [
					{
						"uuid": "081B50DF-F636-4FFF-9323-25EA54633BBF",
						"type": "Mesh",
						"name": "1_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,6.2,0,1],
						"geometry": "E2AA4E97-0106-4697-BA9F-51BD2C168B3C",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "50295E32-9E99-4992-B2CB-7759414C77EA",
						"type": "Mesh",
						"name": "2_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,6.2,3,1],
						"geometry": "E2AA4E97-0106-4697-BA9F-51BD2C168B3C",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "FCC728D6-A896-4333-8C0F-D363AD915A86",
						"type": "Mesh",
						"name": "3_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,5,0.5,1],
						"geometry": "0D7558F7-4B9F-4A5F-A290-8799EDB67DB1",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "BD7ADEBA-5AEA-4605-B927-EE62E2237396",
						"type": "Mesh",
						"name": "4_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,5,2.5,1],
						"geometry": "0D7558F7-4B9F-4A5F-A290-8799EDB67DB1",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "3042F586-07A7-4DEE-A2B8-7C590844A9B3",
						"type": "Mesh",
						"name": "5_H_frame",
						"layers": 1,
						"matrix": [0,0,1,0,-1,0,0,0,0,-1,0,0,-3.5,6.846346,1.5,1],
						"geometry": "4CF57387-5CDC-4641-A9DD-B972BD5DC3EC",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "9D250F7C-9979-4B5A-8D94-11000294FE5B",
						"type": "Mesh",
						"name": "6_H_frame",
						"layers": 1,
						"matrix": [0,0,1,0,-1,0,0,0,0,-1,0,0,-3.5,4.049669,1.5,1],
						"geometry": "4CF57387-5CDC-4641-A9DD-B972BD5DC3EC",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					}]
			},
			{
				"uuid": "59C30B46-7EEB-484A-B238-BA557641C4A4",
				"type": "Group",
				"name": "frame_back",
				"layers": 1,
				"matrix": [1,0,0,0,0,1,0,0,0,0,1,0,6.5,0,0,1],
				"children": [
					{
						"uuid": "6656E0C2-62E8-4557-8FFC-549D6EE37A7D",
						"type": "Mesh",
						"name": "1_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,6.2,0,1],
						"geometry": "E2AA4E97-0106-4697-BA9F-51BD2C168B3C",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "2C748DF6-1E14-4F84-BCA6-662C4A78468D",
						"type": "Mesh",
						"name": "2_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,6.2,3,1],
						"geometry": "E2AA4E97-0106-4697-BA9F-51BD2C168B3C",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "54F1040B-BB3A-41D8-9C5D-531DE1C244B4",
						"type": "Mesh",
						"name": "3_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,5,0.5,1],
						"geometry": "0D7558F7-4B9F-4A5F-A290-8799EDB67DB1",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "35DDEB33-0BB5-4402-9F25-99C1CD62E3A4",
						"type": "Mesh",
						"name": "4_H_frame",
						"layers": 1,
						"matrix": [0,1,0,0,-1,0,0,0,0,0,1,0,-3.5,5,2.5,1],
						"geometry": "0D7558F7-4B9F-4A5F-A290-8799EDB67DB1",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "9431CCAF-D3BE-4462-86A2-A88E390B7471",
						"type": "Mesh",
						"name": "5_H_frame",
						"layers": 1,
						"matrix": [0,0,1,0,-1,0,0,0,0,-1,0,0,-3.5,6.846346,1.5,1],
						"geometry": "4CF57387-5CDC-4641-A9DD-B972BD5DC3EC",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					},
					{
						"uuid": "DFAE2190-91A3-4129-8B02-ADFF884E0292",
						"type": "Mesh",
						"name": "6_H_frame",
						"layers": 1,
						"matrix": [0,0,1,0,-1,0,0,0,0,-1,0,0,-3.5,4.049669,1.5,1],
						"geometry": "4CF57387-5CDC-4641-A9DD-B972BD5DC3EC",
						"material": "580993CA-E67F-4D09-AF86-D7BBAE5F9E65"
					}]
			}],
		"background": 11184810,
		"fog": {
			"type": "Fog",
			"color": 16744448,
			"near": 0.1,
			"far": 50
		}
	}
}
`
