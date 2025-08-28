use byteorder::{BigEndian, ReadBytesExt};
use std::io::{self, Read, Seek};

pub type FourCC = String;

fn read_fourcc<T: Read>(buf: &mut T) -> io::Result<FourCC> {
    read_string(buf, 4)
}

fn read_string<T: Read>(buf: &mut T, len: usize) -> io::Result<String> {
    let mut asset_type = vec![0; len];
    buf.read_exact(&mut asset_type)?;
    Ok(String::from_utf8_lossy(&asset_type).into())
}

pub struct Resource {
    pub asset_type: FourCC,
    pub size: u32,
    pub offset: u32,
}

pub struct Pak {
    pub resources: Vec<Resource>,
}

impl Resource {
    pub fn read<T: Read + Seek>(buf: &mut T) -> io::Result<Self> {
        buf.seek_relative(4)?; // Skip isCompressed
        let asset_type = read_fourcc(buf)?;
        buf.seek_relative(4)?; // Skip assetId
        let size = buf.read_u32::<BigEndian>()?;
        let offset = buf.read_u32::<BigEndian>()?;
        Ok(Self {
            asset_type,
            size,
            offset,
        })
    }
}

impl Pak {
    pub fn read<T: Read + Seek>(buf: &mut T) -> io::Result<Self> {
        let major = buf.read_u16::<BigEndian>()?;
        let minor = buf.read_u16::<BigEndian>()?;
        if major != 3 || minor != 5 {
            return Err(io::Error::new(
                io::ErrorKind::InvalidData,
                "Invalid PAK version",
            ));
        }
        buf.seek_relative(4)?; // Skip unused
        let named_count = buf.read_u32::<BigEndian>()? as usize;
        for _ in 0..named_count {
            // We dont care about named resources, skip them
            buf.seek_relative(8)?;
            let len = buf.read_u32::<BigEndian>()?;
            buf.seek_relative(len as i64)?;
        }
        let resource_count = buf.read_u32::<BigEndian>()? as usize;
        let mut resources = Vec::with_capacity(named_count);
        for _ in 0..resource_count {
            resources.push(Resource::read(buf)?);
        }
        Ok(Self { resources })
    }
}
