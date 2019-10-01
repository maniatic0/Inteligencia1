"""PSVN Opinionated Serializer"""

class psvnSerializer(object):
    """
    Handles PSVN Serialization (Opinionated). We don't use the xN domain\n 
    It is the programmer responsability to call the serialization in order.\n"""

    """Marker for comments"""
    COMMENT_MARKER = "#"

    """Marker for Domains"""
    DOMAIN_MARKER = "DOMAIN"

    """Marker for Operators"""
    OPERATOR_MARKER = "=>"

    """Marker for Labels"""
    LABEL_MARKER = "LABEL"

    """Marker for Costs"""
    COST_MARKER = "COST"

    """Separator for State Members"""
    STATE_SEP = " "

    """Marker for Goals"""
    GOAL_MARKER = "GOAL"
    
    @staticmethod
    def StateToString(state):
        """Converts a State to a String"""
        finalState = [str(val).lower() for val in state]
        return psvnSerializer.STATE_SEP.join(finalState)

    def __init__(self, file):
        """Constructor. file is the file to send the output, this class doesn't own the resource, it just uses it"""
        # We don't own the file, we just use it
        self.file = file
        # Number of state variables we have, so we don't change it by accident
        self.numberOfStateVariables = 0
        # Custom Domains We use
        self.domains = {}
        # State Composition
        self.stateComposition = []

    def AddComment(self, comment):
        """Adds a comment to the serialized file"""
        print("{} {}".format(psvnSerializer.COMMENT_MARKER, comment), file=self.file)

    def AddDomain(self, domainName, domainValues, comment=""):
        """Add a Domain with a name and Values (both non empty). Optionally it can have a comment"""

        assert len(domainName) > 0, "Empty Domain Name"

        assert len(domainValues) > 0, "Empty Domain Values"

        if comment != "":
            self.AddComment(comment)

        finalDomainName = domainName.lower()

        finalDomainValues = [str(val).lower() for val in domainValues]

        self.domains[finalDomainName] = set(finalDomainValues)

        print(
            "{} {} {}".format(
                psvnSerializer.DOMAIN_MARKER,
                finalDomainName,
                psvnSerializer.STATE_SEP.join(finalDomainValues),
            ),
            file=self.file,
        )

    def AddNumberOfStateVariables(self, number, comment=""):
        """Adds the number of State Variables. It must be a positive non zero number. Optionally it can have a comment"""

        assert (
            number > 0
        ), "The number of State Variables must be a positive non zero number. '{}' is not valid".format(
            number
        )

        if comment != "":
            self.AddComment(comment)

        self.numberOfStateVariables = number

        print(str(number), file=self.file)

    def AddStateComposition(self, domains, comment=""):
        """Adds the state composition. It must be an array of either positive non-zero integers or domain's names (strings)"""
        assert len(domains) == self.numberOfStateVariables

        if comment != "":
            self.AddComment(comment)

        finalDomainName = ""

        for domain in domains:
            if isinstance(domain, (int,)):
                assert (
                    domain > 0
                ), "Invalid numeric Domain '{}', it must be greater than 0".format(
                    domain
                )
                finalDomainName = str(domain)
                self.stateComposition.append(domain)
            else:
                # Custom Domain
                finalDomainName = domain.lower()
                assert (
                    finalDomainName in self.domains
                ), "Custom Domain '{}' not declared".format(finalDomainName)
                self.stateComposition.append(domain)

            print(finalDomainName, end=psvnSerializer.STATE_SEP, file=self.file)

        print("", file=self.file)

    def CheckStatePositionIsValid(self, position, val):
        """
        Checks if the Value is valid according to the Domain at a position of the State Composition.\n
        val must be an int or a string. DO NOT use for now, have to fix many rules
        """
        domain = self.stateComposition[position]

        if isinstance(domain, (int,)):

            assert isinstance(
                val, (int,)
            ), "Domain at position {} is an int, but the value assigned '{}' is not".format(
                position, val
            )

            assert (
                0 <= val and val < domain
            ), "Value '{}'outside of its integer domain of {}".format(val, domain)
        else:
            # Custom Domain
            finalvalueName = val.lower()
            assert (
                finalvalueName in self.domains[domain]
            ), "Value'{}' Outside of it's Custom Domain '{}'".format(
                finalvalueName, domain
            )

    def CheckStateIsValid(self, state):
        """
        Checks if the State is valid according to the State Composition.\n
        state must be an array of ints or strings. DO NOT use for now, have to fix many rules
        """

        assert len(state) > 0, "Invalid Empty State"

        assert (
            len(state) == self.numberOfStateVariables
        ), "State '{}' contain less than {} variables, as specified in the State Composition".format(
            state, self.numberOfStateVariables
        )

        for i in range(len(state)):
            self.CheckStatePositionIsValid(i, state[i])

    def AddRule(self, state1, state2, label="", cost=1, comment=""):
        """Adds a Rule from state1 to state2 with a possible labe and a cost"""
        assert (
            cost >= 0
        ), "The cost must be non negative, but the passed value was {}".format(cost)

        assert (
            len(state1) == self.numberOfStateVariables
        ), "State '{}' contain less than {} variables, as specified in the State Composition".format(
            state1, self.numberOfStateVariables
        )

        assert (
            len(state2) == self.numberOfStateVariables
        ), "State '{}' contain less than {} variables, as specified in the State Composition".format(
            state2, self.numberOfStateVariables
        )

        finalState1 = psvnSerializer.StateToString(state1)
        finalState2 = psvnSerializer.StateToString(state2)

        if comment != "":
            self.AddComment(comment)

        print(
            "{} {} {}".format(finalState1, psvnSerializer.OPERATOR_MARKER, finalState2),
            end="",
            file=self.file,
        )

        if label != "":
            print(
                " {} {}".format(psvnSerializer.LABEL_MARKER, label),
                end="",
                file=self.file,
            )

        print(" {} {}".format(psvnSerializer.COST_MARKER, cost), file=self.file)

    def AddGoal(self, state, comment=""):
        if comment != "":
            self.AddComment(comment)

        assert (
            len(state) == self.numberOfStateVariables
        ), "State '{}' contain less than {} variables, as specified in the State Composition".format(
            state, self.numberOfStateVariables
        )

        print(
            "{} {}".format(
                psvnSerializer.GOAL_MARKER, psvnSerializer.StateToString(state)
            ),
            file=self.file,
        )