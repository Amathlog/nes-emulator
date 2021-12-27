#include <core/processor2A03.h>

using NesEmulator::Processor2A03;

void Processor2A03::Clock()
{

}

void Processor2A03::WriteCPU(uint16_t addr, uint8_t data)
{
	if (addr >= 0x4000 && addr <= 0x4007)
	{
		// Pulse
		PulseRegister& currPulseRegister = ((addr & 0x0004) > 0) ? m_pulseRegister2 : m_pulseRegister1;
		switch (addr & 0x0003)
		{
		case 0:
			currPulseRegister.duty = data >> 6;
			currPulseRegister.enveloppeLoop = (data & 0x20) > 0;
			currPulseRegister.constantVolume = (data & 0x10) > 0;
			currPulseRegister.volumeEnveloppe = (data & 0x0F);
			break;
		case 1:
			currPulseRegister.sweepEnable = (data & 0x80) > 0;
			currPulseRegister.sweepPeriod = (data & 0x70) >> 4;
			currPulseRegister.sweepNegate = (data & 0x08) > 0;
			currPulseRegister.sweepShift = (data & 0x07);
			break;
		case 2:
			currPulseRegister.timer = (currPulseRegister.timer & 0xFF00) | (uint16_t)data;
			break;
		case 3:
			currPulseRegister.lengthCounter = (data & 0xF8) >> 3;
			currPulseRegister.timer = (uint16_t)(data & 0x07) << 8 | (currPulseRegister.timer & 0x00FF);
			break;
		}
	}
	else if (addr >= 0x4008 && addr <= 0x400B)
	{
		// Triangle
		switch (addr & 0x0003)
		{
		case 0:
			m_triangleRegister.control = data >> 7;
			m_triangleRegister.linearCounterLoad = data & 0x7F;
			break;
		case 1:
			// Unused
			break;
		case 2:
			m_triangleRegister.timer = (m_triangleRegister.timer & 0xFF00) | (uint16_t)data;
			break;
		case 3:
			m_triangleRegister.lengthCounterLoad = (data & 0xF8) >> 3;
			m_triangleRegister.timer = (uint16_t)(data & 0x07) << 8 | (m_triangleRegister.timer & 0x00FF);
			break;
		}
	}
	else if (addr >= 0x400C && addr <= 0x400F)
	{
		// Noise
		switch (addr & 0x0003)
		{
		case 0:
			m_noiseRegister.enveloppeLoop = (data & 0x20) >> 5;
			m_noiseRegister.constantVolume = (data & 0x10) > 0;
			m_noiseRegister.volumeEnveloppe = (data & 0x0F);
			break;
		case 1:
			// Unused
			break;
		case 2:
			m_noiseRegister.loopNoise = (data & 0x80) > 0;
			m_noiseRegister.noisePeriod = (data & 0x0F);
			break;
		case 3:
			m_noiseRegister.lengthCounterLoad = (data & 0xF8) >> 3;
			break;
		}
	}
	else if (addr >= 0x4010 && addr <= 0x4013)
	{
		// DMC
		switch (addr & 0x0003)
		{
		case 0:
			m_dmcRegister.irqEnable = (data & 0x80) > 0;
			m_dmcRegister.loop = (data & 0x40) > 0;
			m_dmcRegister.frequency = (data & 0x0F);
			break;
		case 1:
			m_dmcRegister.loadCounter = (data & 0x7F);
			break;
		case 2:
			m_dmcRegister.sampleAddress = data;
			break;
		case 3:
			m_dmcRegister.sampleLength = data;
			break;
		}
	}
	else if (addr == 0x4015)
	{
		// Status
		m_statusRegister.flags = (m_statusRegister.flags & 0xE0) | (data & 0x1F);
	}
	else if (addr == 0x4017)
	{
		// Frame counter
		m_frameCounterRegister.flags = data;
	}
}

uint8_t Processor2A03::ReadCPU(uint16_t addr)
{
	if (addr == 0x4015)
	{
		// Status
		return m_statusRegister.flags;
	}

	return 0;
}

void Processor2A03::Reset()
{
	m_pulseRegister1.Reset();
	m_pulseRegister2.Reset();
	m_triangleRegister.Reset();
	m_noiseRegister.Reset();
	m_dmcRegister.Reset();
	m_statusRegister.flags = 0;
	m_frameCounterRegister.flags = 0;
}

void Processor2A03::SerializeTo(Utils::IWriteVisitor& visitor) const
{
	m_pulseRegister1.SerializeTo(visitor);
	m_pulseRegister2.SerializeTo(visitor);
	m_triangleRegister.SerializeTo(visitor);
	m_noiseRegister.SerializeTo(visitor);
	m_dmcRegister.SerializeTo(visitor);

	visitor.WriteValue(m_statusRegister.flags);
	visitor.WriteValue(m_frameCounterRegister.flags);
}

void Processor2A03::DeserializeFrom(Utils::IReadVisitor& visitor)
{
	m_pulseRegister1.DeserializeFrom(visitor);
	m_pulseRegister2.DeserializeFrom(visitor);
	m_triangleRegister.DeserializeFrom(visitor);
	m_noiseRegister.DeserializeFrom(visitor);
	m_dmcRegister.DeserializeFrom(visitor);

	visitor.ReadValue(m_statusRegister.flags);
	visitor.ReadValue(m_frameCounterRegister.flags);
}