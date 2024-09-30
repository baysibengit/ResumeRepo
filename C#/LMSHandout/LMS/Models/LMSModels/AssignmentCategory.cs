using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class AssignmentCategory
{
    public uint Id { get; set; }

    public string Name { get; set; } = null!;

    public byte Weight { get; set; }

    public uint Class { get; set; }

    public virtual ICollection<Assignment> Assignments { get; set; } = new List<Assignment>();

    public virtual Class ClassNavigation { get; set; } = null!;
}
